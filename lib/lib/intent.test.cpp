#include <lib/exception.hpp>
#include <lib/intent.hpp>
#include <lib/raise.hpp>
#include <lib/source_location.hpp>

#include <boost/test/unit_test.hpp>

#include <future>
#include <numeric>
#include <string>

using namespace std::string_literals;

namespace
{
using ::xzr::error::intent;
using ::xzr::error::intent_f;
using ::xzr::error::intention_stack;
using ::xzr::ext::source_location;

using Error = std::runtime_error;

int a(int i)
{
    const auto& _{intent().capture("a(", i, ')')};
    if (i % 2 != 0)
        throw Error{"odd number not allowed"};
    return i;
}

std::pair<int, std::string> a_range(int min, int max)
{
    auto accu{int{}};
    auto msg{""s};
    try
    {
        const auto& _a{intent().capture("a_range(", min, ", ", max, ")")};
        for (int idx{min}; idx < max; ++idx)
        {
            const auto& _b{intent().capture("accu:", accu, " idx:", idx)};
            accu += a(idx);
        }
    }
    catch (...)
    {
        msg += std::accumulate(intention_stack().cbegin(),
                               intention_stack().cend(),
                               ""s,
                               [](auto accu, const auto& m) {
                                   accu += m.msg() + '\n';
                                   return accu;
                               });
        intention_stack().clear();
    }
    return {accu, msg};
}

struct cm_spy
{
    explicit cm_spy(std::string* s)
        : calls{s}
    {
    }

    cm_spy(const cm_spy& a)
        : calls{a.calls}
    {
        *calls += "_copy_";
    }

    cm_spy(cm_spy&& a) noexcept
        : calls{a.calls}
    {
        *calls += "_move_";
    }

    cm_spy& operator=(const cm_spy& a)
    {
        calls = a.calls;
        *calls += "_copy=_";
        return *this;
    }

    cm_spy& operator=(cm_spy&& a) noexcept
    {
        calls = a.calls;
        *calls += "_move=_";
        return *this;
    }

    std::string* calls;
};

std::ostream& operator<<(std::ostream& s, const cm_spy&)
{
    s << "cm_spy";
    return s;
}

struct test_intent
{
    test_intent()
    {
        intention_stack().clear();
    }

    ~test_intent()
    {
        intention_stack().clear();
    }
};

BOOST_FIXTURE_TEST_SUITE(intent_tests, test_intent)

BOOST_AUTO_TEST_CASE(no_intent_on_success)
{
    const auto& i{intent().capture("start", 0)};
    a(0);
    BOOST_TEST(intention_stack().empty());
}

BOOST_AUTO_TEST_CASE(intent_on_failure)
{
    const auto loc{source_location::current()};
    try
    {
        const auto& i{intent().capture("test a(", -1, ')')};
        a(-1);
    }
    catch (...)
    {
    }
    BOOST_REQUIRE(!intention_stack().empty());
    BOOST_REQUIRE(intention_stack().size() == 2);

    BOOST_TEST(intention_stack()[0].msg() == "a(-1)");
    BOOST_TEST(intention_stack()[0].where().file_name() == loc.file_name());
    BOOST_TEST(intention_stack()[0].where().line() == 25u);
    BOOST_TEST(intention_stack()[0].where().function_name() == "a");

    BOOST_TEST(intention_stack()[1].msg() == "test a(-1)");
    BOOST_TEST(intention_stack()[1].where().file_name() == loc.file_name());
    BOOST_TEST(intention_stack()[1].where().line() == loc.line() + 3);
    BOOST_TEST(intention_stack()[1].where().function_name() ==
               loc.function_name());
}

BOOST_AUTO_TEST_CASE(intent_copies_lvalue)
{
    auto calls{""s};
    auto spy{cm_spy{&calls}};

    try
    {
        const auto i{intent().capture(spy)};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "_copy__move_");
    BOOST_TEST(!intention_stack().empty());
}

BOOST_AUTO_TEST_CASE(intent_moves_rvalue)
{
    auto calls{""s};

    try
    {
        const auto i{intent().capture(cm_spy{&calls})};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "_move__move_");
    BOOST_TEST(!intention_stack().empty());
}

BOOST_AUTO_TEST_CASE(intent_is_snapshot_by_default)
{
    try
    {
        auto a{0};
        const auto i{intent().capture(a)};
        ++a;
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_REQUIRE(intention_stack().size() == 1);
    BOOST_TEST(intention_stack()[0].msg() == "0");
}

BOOST_AUTO_TEST_CASE(intent_can_take_ref)
{
    try
    {
        auto a{0};
        const auto i{intent().capture(std::cref(a))};
        ++a;
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_REQUIRE(intention_stack().size() == 1);
    BOOST_TEST(intention_stack()[0].msg() == "1");
}

BOOST_AUTO_TEST_CASE(intent_from_a_range)
{
    {
        const auto& a{a_range(0, 1)};
        BOOST_TEST(a.second == "");
    }
    {
        const auto& a{a_range(-1, 0)};
        BOOST_TEST(a.second == "a(-1)\n"
                               "accu:0 idx:-1\n"
                               "a_range(-1, 0)\n");
    }
}

BOOST_AUTO_TEST_CASE(intent_from_multiple_threads)
{
    auto results{std::vector<std::future<std::pair<int, std::string>>>{}};

    for (auto i{int{-50}}; i < 50; i += 10)
    {
        results.push_back(std::async(a_range, i, i + 10));
    }

    for (auto& f : results)
    {
        f.wait();
    }

    auto msg{""s};
    for (auto& f : results)
    {
        msg += f.get().second;
    }

    BOOST_TEST(!msg.empty());
}

BOOST_AUTO_TEST_CASE(intent_with_lambda_capture)
{
    auto calls{""s};
    auto spy{cm_spy{&calls}};

    try
    {
        const auto i{intent_f().on_fail_msg([&]() { return *spy.calls; })};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(!intention_stack().empty());
    BOOST_TEST(intention_stack()[0].msg() == "");
}

BOOST_AUTO_TEST_SUITE_END()
}
