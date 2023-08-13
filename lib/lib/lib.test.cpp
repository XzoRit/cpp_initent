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
message create_msg(const std::tuple<const A&...>& tup)
{
    return std::apply(
        [](auto&&... args) {
            std::ostringstream str{};
            (str << ... << args);
            return str.str();
        },
        tup);
}

template <class... A>
struct [[maybe_unused]] intent
{
    explicit intent(std::tuple<const A&...> t)
        : tup{t}
    {
    }

    ~intent()
    {
        if (std::uncaught_exceptions() <= except_count)
            return;
        try
        {
            const auto& full_msg{create_msg(tup)};
            msgs().push_back(full_msg);
        }
        catch (...)
        {
        }
    }
    int except_count{std::uncaught_exceptions()};
    std::tuple<const A&...> tup;
};

int a(int i)
{
    intent in{ctie("intent a", " with i:", i)};
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
    intent i{ctie("start", 0)};
    a(0);
    BOOST_TEST(msgs().empty());
}

BOOST_AUTO_TEST_CASE(intent_on_failure)
{
    try
    {
        intent ii{ctie("a(-1)", -1)};
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
