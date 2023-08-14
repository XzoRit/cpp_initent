#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

namespace
{
using namespace std::string_literals;

struct cm_spy
{
    explicit cm_spy(std::string* s)
        : calls{s}
    {
    }

    cm_spy(const cm_spy&)
    {
        *calls += "cc";
    }

    cm_spy(cm_spy&&)
    {
        *calls += "mc";
    }

    cm_spy& operator=(const cm_spy&)
    {
        *calls += "ca";
        return *this;
    }

    cm_spy& operator=(cm_spy&&)
    {
        *calls += "ma";
        return *this;
    }

    std::string* calls;
};

std::ostream& operator<<(std::ostream& s, const cm_spy&)
{
    s << "cm_spy";
    return s;
}

using message = std::string;
using messages = std::vector<message>;

messages& msgs()
{
    static messages ms{};
    return ms;
}

template <class... A>
[[nodiscard]] message msg_from_ostringstream(const A&... args)
{
    std::ostringstream str{};
    (str << ... << args);
    return str.str();
}

template <class Tup>
struct [[maybe_unused]] intent
{
    explicit intent(Tup args)
        : tup{args}
    {
    }

    ~intent()
    {
        if (std::uncaught_exceptions() <= except_count)
            return;
        try
        {
            msgs().push_back(std::apply(
                [](const auto&... args) {
                    return msg_from_ostringstream(args...);
                },
                tup));
        }
        catch (...)
        {
        }
    }

    int except_count{std::uncaught_exceptions()};
    Tup tup;
};

template <class... A>
[[nodiscard]] auto make_intent(const A&... args)
{
    return intent{std::make_tuple(args...)};
}

[[nodiscard]] std::string rvalue_intent_msg_for_a(int i)
{
    return "intent a with i:"s + std::to_string(i);
}

int a(int i)
{
    const auto& in{make_intent(rvalue_intent_msg_for_a(i))};
    if (i < 0)
        throw std::runtime_error{"i < 0"};
    return i;
}

BOOST_AUTO_TEST_SUITE(intent_tests)

BOOST_AUTO_TEST_CASE(intent_is_empty)
{
    BOOST_TEST(msgs().empty());
}

BOOST_AUTO_TEST_CASE(no_intent_on_success)
{
    const auto& i{make_intent("start", 0)};
    a(0);
    BOOST_TEST(msgs().empty());
}

BOOST_AUTO_TEST_CASE(intent_on_failure)
{
    try
    {
        const auto& i{make_intent("a(-1)", -1)};
        a(-1);
    }
    catch (const std::exception& e)
    {
        BOOST_TEST(!msgs().empty());
        BOOST_TEST(msgs().size() == 2);
        BOOST_TEST(msgs()[0] == "intent a with i:-1");
        BOOST_TEST(msgs()[1] == "a(-1)-1");

        msgs().clear();
    }
}

BOOST_AUTO_TEST_CASE(intent_only_views_data)
{
    auto calls{""s};
    auto spy{cm_spy{&calls}};

    try
    {
        const auto i{make_intent(spy)};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "");
    BOOST_TEST(!msgs().empty());
}

BOOST_AUTO_TEST_SUITE_END()
}
