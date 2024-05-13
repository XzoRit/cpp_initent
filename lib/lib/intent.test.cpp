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
using ::xzr::error::intention_container;
using ::xzr::error::intention_stack;
using ::xzr::ext::source_location;

using Error = std::runtime_error;

int a(int i)
{
    const auto& _{intent().capture("check if number is odd", i)};
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
        const auto& _{intent().capture("sum number from min to max", min, max)};
        for (int idx{min}; idx < max; ++idx)
        {
            const auto& _{intent().capture("partial sum", accu, idx)};
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
        intention_container().clear();
    }

    ~test_intent()
    {
        intention_container().clear();
        intention_stack().clear();
    }

    static std::string intent_container_to_string()
    {
        std::ostringstream str{};
        ::xzr::error::dump_intents(str);
        return str.str();
    }
};

BOOST_FIXTURE_TEST_SUITE(intent_tests, test_intent)

BOOST_AUTO_TEST_CASE(no_intent_on_success)
{
    const auto& _{intent().capture("no intent on success")};
    a(0);
    BOOST_TEST(intention_stack().empty());
}

BOOST_AUTO_TEST_CASE(intent_on_failure)
{
    const auto loc{source_location::current()};
    try
    {
        const auto& _{intent().capture("intent on failure")};
        a(-1);
    }
    catch (...)
    {
    }
    const auto intents_str{intent_container_to_string()};
    BOOST_TEST(intents_str.contains("check if number is odd -1\n"),
               intents_str);
    BOOST_TEST(intents_str.contains("intent on failure\n"), intents_str);

    BOOST_REQUIRE(!intention_stack().empty());
    BOOST_REQUIRE_EQUAL(intention_stack().size(), 2u);

    BOOST_TEST(intention_stack()[0].msg() == "check if number is odd -1");
    BOOST_TEST(intention_stack()[0].where().file_name() == loc.file_name());
    BOOST_TEST(intention_stack()[0].where().line() == 26u);
    BOOST_TEST(intention_stack()[0].where().function_name() == "a");

    BOOST_TEST(intention_stack()[1].msg() == "intent on failure");
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
        const auto i{intent().capture("lvalues are copied", spy)};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "_copy__move__move_");
    BOOST_TEST(!intention_stack().empty());
}

BOOST_AUTO_TEST_CASE(intent_moves_rvalue)
{
    auto calls{""s};

    try
    {
        const auto i{intent().capture("rvalues are moved", cm_spy{&calls})};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "_move__move__move_");
    BOOST_TEST(!intention_stack().empty());
}

BOOST_AUTO_TEST_CASE(intent_is_snapshot_by_default)
{
    try
    {
        auto a{0};
        const auto i{intent().capture("intent takes snapshot", a)};
        ++a;
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(
        intent_container_to_string().ends_with("intent takes snapshot 0\n"));

    BOOST_REQUIRE(intention_stack().size() == 1);
    BOOST_TEST(intention_stack()[0].msg() == "intent takes snapshot 0");
}

BOOST_AUTO_TEST_CASE(intent_can_take_ref)
{
    try
    {
        auto a{0};
        const auto i{
            intent().capture("intent can take references", std::cref(a))};
        ++a;
        throw 42;
    }
    catch (...)
    {
    }

    const auto intents_str{intent_container_to_string()};
    BOOST_TEST(intents_str.contains("intent can take references 1\n"),
               intents_str);

    BOOST_REQUIRE(intention_stack().size() == 1);
    BOOST_TEST(intention_stack()[0].msg() == "intent can take references 1");
}

BOOST_AUTO_TEST_CASE(intent_from_a_range)
{
    {
        const auto& a{a_range(0, 1)};
        BOOST_TEST(a.second == "");
    }
    {
        const auto& a{a_range(-1, 0)};
        BOOST_TEST(a.second == "check if number is odd -1\n"
                               "partial sum 0 -1\n"
                               "sum number from min to max -1 0\n");
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

    BOOST_CHECK_EQUAL(intent_container_to_string(), "");

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
}

BOOST_AUTO_TEST_SUITE_END()
}
