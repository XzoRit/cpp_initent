#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

namespace
{

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

template <class... A>
constexpr std::tuple<const A&...> ctie(const A&... args)
{
    return {args...};
}

using namespace std::string_literals;

using message = std::string;
using messages = std::vector<message>;

messages& msgs()
{
    static messages ms{};
    return ms;
}

template <class... A>
message msg_from_ostringstream(A&&... args)
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
            const auto& full_msg{std::apply(
                [](auto&&... args) { return msg_from_ostringstream(args...); },
                tup)};
            msgs().push_back(full_msg);
        }
        catch (...)
        {
        }
    }
    int except_count{std::uncaught_exceptions()};
    Tup tup;
};

template <class... A>
auto make_intent(A&&... args)
{
    return intent{ctie(args...)};
}

int a(int i)
{
    const auto& in{make_intent("intent a", " with i:", i)};
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
