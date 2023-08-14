#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

namespace
{
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

template <class... A>
struct [[maybe_unused]] intent
{
    explicit intent(const A&... args)
        : tup{ctie(args...)}
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
    std::tuple<const A&...> tup;
};

template <class... A>
auto make_intent(A&&... args)
{
    return intent{args...};
}

int a(int i)
{
    const auto& in{intent{"intent a", " with i:", i}};
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
    const auto& i{intent{"start", 0}};
    a(0);
    BOOST_TEST(msgs().empty());
}

BOOST_AUTO_TEST_CASE(intent_on_failure)
{
    try
    {
        const auto& i{intent{"a(-1)", -1}};
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

BOOST_AUTO_TEST_SUITE_END()
}
