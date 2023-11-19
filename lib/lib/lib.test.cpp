#include <boost/test/unit_test.hpp>

#include <future>
#include <numeric>
#include <string>
#include <vector>

namespace
{
namespace test = boost::unit_test;

using namespace std::string_literals;

using message = std::string;
using messages = std::vector<message>;

messages& msgs()
{
    thread_local static messages ms{};
    return ms;
}

template <class Stream, class... A>
Stream& stream_args_into(Stream& str, const A&... args)
{
    (str << ... << args);
    return str;
}

template <class... A>
[[nodiscard]] message msg_from_ostringstream(const A&... args)
{
    std::ostringstream str{};
    return stream_args_into(str, args...).str();
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
    const auto& in{make_intent("a(", i, ')')};
    if (i % 2 != 0)
        throw std::runtime_error{"odd number not allowed"};
    return i;
}

std::pair<int, std::string> a_range(int min, int max)
{
    auto accu{int{}};
    auto msg{""s};
    for (int idx{min}; idx < max; ++idx)
    {
        try
        {
            const auto& i{make_intent("a_range(", min, ", ", max, ")")};
            const auto& ii{make_intent("accu:", accu, " idx:", idx)};
            accu += a(idx);
        }
        catch (...)
        {
            msg += std::accumulate(msgs().cbegin(),
                                   msgs().cend(),
                                   ""s,
                                   [](auto accu, const auto& s) {
                                       accu += s + '\n';
                                       return accu;
                                   });
            msgs().clear();
        }
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
        *calls += "cc";
    }

    cm_spy(cm_spy&& a)
        : calls{a.calls}
    {
        *calls += "mc";
    }

    cm_spy& operator=(const cm_spy& a)
    {
        calls = a.calls;
        *calls += "ca";
        return *this;
    }

    cm_spy& operator=(cm_spy&& a)
    {
        calls = a.calls;
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
        const auto& i{make_intent("test a(", -1, ')')};
        a(-1);
    }
    catch (...)
    {
    }
    BOOST_REQUIRE(!msgs().empty());
    BOOST_REQUIRE(msgs().size() == 2);
    BOOST_TEST(msgs()[0] == "a(-1)");
    BOOST_TEST(msgs()[1] == "test a(-1)");
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

BOOST_AUTO_TEST_SUITE_END()
}
