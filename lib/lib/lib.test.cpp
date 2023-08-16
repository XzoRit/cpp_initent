#include <boost/test/unit_test.hpp>

#include <future>
#include <numeric>
#include <string>
#include <vector>

namespace
{
namespace test = boost::unit_test;

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
[[nodiscard]] auto make_intent(A&&... args)
{
    return intent{std::tie(args...)};
}

int a(int i)
{
    const auto& in{make_intent("intent a with i:", i)};
    if (i < 0)
        throw std::runtime_error{"i < 0"};
    return i;
}

int a_range(int min, int max)
{
    const auto& i{make_intent("a(", min, ", ", max, ")")};
    auto accu{int{}};
    for (; min < max; ++min)
    {
        const auto& ii{make_intent("accu:", accu, " min:", min)};
        accu += a(min);
    }
    return accu;
}

struct test_intent
{
    test_intent()
    {
        msgs().clear();
    }

    ~test_intent()
    {
        msgs().clear();
    }
};

BOOST_FIXTURE_TEST_SUITE(intent_tests, test_intent)

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

BOOST_AUTO_TEST_CASE(intent_copies_data_if_rvalue, *test::disabled())
{
    auto calls{""s};

    try
    {
        const auto i{make_intent(cm_spy{&calls})};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "cc");
    BOOST_TEST(!msgs().empty());
}

BOOST_AUTO_TEST_CASE(intent_from_multiple_threads)
{
    auto results{std::vector<std::future<int>>{}};

    for (auto i{int{-50}}; i < 50; i += 10)
    {
        results.push_back(std::async(a_range, i, i + 10));
    }

    for (auto& f : results)
        f.wait();

    BOOST_TEST(!msgs().empty());
    BOOST_TEST(std::accumulate(msgs().cbegin(),
                               msgs().cend(),
                               ""s,
                               [](auto accu, const auto& s) {
                                   accu += s + '\n';
                                   return accu;
                               }) == "");
}

BOOST_AUTO_TEST_SUITE_END()
}
