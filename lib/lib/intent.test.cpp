#include <lib/exception.hpp>
#include <lib/intent.hpp>
#include <lib/raise.hpp>

#include <boost/test/unit_test.hpp>

#include <string>

using namespace std::string_literals;

namespace
{
using ::xzr::error::eager_fmt::intent;
using ::xzr::error::eager_fmt::intention_stack;

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

int throw_on_odd(int i)
{
    const auto& _{intent().capture("check if number is odd", i)};
    if (i % 2 != 0)
        throw std::runtime_error{"odd number not allowed"};
    return i;
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

    template <class Func>
    static std::string intention_string(Func&& dump)
    {
        std::ostringstream str{};
        dump(str);
        return str.str();
    }
};

BOOST_FIXTURE_TEST_SUITE(intent_tests, test_intent)

BOOST_AUTO_TEST_CASE(no_intent_on_success)
{
    {
        using ::xzr::error::eager_fmt::dump_intention_stack;
        using ::xzr::error::eager_fmt::intent;

        const auto& _{intent().capture("no intent on success")};
        throw_on_odd(0);

        const auto intents_str{intention_string(dump_intention_stack)};
        BOOST_TEST(intents_str.empty());
    }
    {
        using ::xzr::error::eager_fmt::dump_intention_stack;
        using ::xzr::error::lazy_fmt::intent;

        const auto& _{intent().capture("no intent on success")};
        throw_on_odd(0);

        const auto intents_str{intention_string(dump_intention_stack)};
        BOOST_TEST(intents_str.empty());
    }
}

BOOST_AUTO_TEST_CASE(intent_is_snapshot_by_default)
{
    {
        using ::xzr::error::eager_fmt::dump_intention_stack;
        using ::xzr::error::eager_fmt::intent;

        try
        {
            auto a{0};
            const auto i{intent().capture("intent takes snapshot", a)};
            ++a;
            throw_on_odd(a);
        }
        catch (...)
        {
        }

        const auto intents_str{intention_string(dump_intention_stack)};
        BOOST_TEST(!intents_str.empty());
        BOOST_CHECK_EQUAL(*std::prev(intents_str.cend(), 2), '0');
    }
    {
        using ::xzr::error::lazy_fmt::dump_intention_stack;
        using ::xzr::error::lazy_fmt::intent;

        try
        {
            auto a{0};
            const auto i{intent().capture("intent takes snapshot", a)};
            ++a;
            throw_on_odd(a);
        }
        catch (...)
        {
        }

        const auto intents_str{intention_string(dump_intention_stack)};
        BOOST_TEST(!intents_str.empty());
        BOOST_CHECK_EQUAL(*std::prev(intents_str.cend(), 2), '0');
    }
}

BOOST_AUTO_TEST_CASE(intent_can_take_ref)
{
    {
        using ::xzr::error::eager_fmt::dump_intention_stack;
        using ::xzr::error::eager_fmt::intent;

        try
        {
            auto a{0};
            const auto i{
                intent().capture("intent can take references", std::cref(a))};
            ++a;
            throw_on_odd(a);
        }
        catch (...)
        {
        }

        const auto intents_str{intention_string(dump_intention_stack)};
        BOOST_REQUIRE(!intents_str.empty());
        BOOST_CHECK_EQUAL(*std::prev(intents_str.cend(), 2), '1');
    }
    {
        using ::xzr::error::lazy_fmt::intent;

        try
        {
            auto a{0};
            const auto i{intent().capture(
                "this should not compile",
                std::cref(a) /* todo: this should not compile
                              when formatting lazily references are dangling */)};
        }
        catch (...)
        {
        }
    }
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

    BOOST_CHECK_EQUAL(calls, "_copy__move_");
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

    BOOST_CHECK_EQUAL(calls, "_move__move_");
}

BOOST_AUTO_TEST_SUITE_END()
}
