#include <boost/test/unit_test.hpp>

#include <future>
#include <numeric>
#include <string>
#include <vector>

namespace
{
using namespace std::string_literals;

struct source_location
{
    [[nodiscard]] static consteval source_location current(
#if (__has_builtin(__builtin_FILE))
        const char* const _File_ = __builtin_FILE(),
#else
        const char* const _File_,
#endif
#if (__has_builtin(__builtin_LINE))
        const std::uint_least32_t _Line_ = __builtin_LINE(),
#else
        const std::uint_least32_t _Line_,
#endif
#if (__has_builtin(__builtin_COLUMN))
        const std::uint_least32_t _Column_ = __builtin_COLUMN(),
#else
        const std::uint_least32_t _Column_ = 0,
#endif
#if (__has_builtin(__builtin_FUNCSIG))
        const char* const _Function_ = __builtin_FUNCSIG()
#elif (__has_builtin(__builtin_FUNCTION))
        const char* const _Function_ = __builtin_FUNCTION()
#else
        const char* const _Function_ = ""
#endif
            ) noexcept
    {
        source_location _Result{};
        _Result._Line = _Line_;
        _Result._Column = _Column_;
        _Result._File = _File_;
        _Result._Function = _Function_;
        return _Result;
    }

    [[nodiscard]] constexpr source_location() noexcept = default;

    [[nodiscard]] constexpr std::uint_least32_t line() const noexcept
    {
        return _Line;
    }

    [[nodiscard]] constexpr std::uint_least32_t column() const noexcept
    {
        return _Column;
    }

    [[nodiscard]] constexpr const char* file_name() const noexcept
    {
        return _File;
    }

    [[nodiscard]] constexpr const char* function_name() const noexcept
    {
        return _Function;
    }

  private:
    std::uint_least32_t _Line{};
    std::uint_least32_t _Column{};
    const char* _File = "unknown_file";
    const char* _Function = "unknown_func";
};

#define CURRENT_LOC source_location::current(__FILE__, __LINE__, 0, __func__)

std::ostream& operator<<(std::ostream& s, const source_location& l)
{
    s << l.file_name() << ':' << l.line() << ':' << l.column() << ':'
      << l.function_name() << ':';
    return s;
}

template <class Exception>
class [[maybe_unused]] Throw
{
  public:
    explicit Throw(source_location srcLoc = source_location::current())
        : m_srcLoc{srcLoc}
    {
    }

#if defined(DFL_CONFIG_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~Throw() noexcept(false)
    {
        throw Exception{m_stream.str().c_str(), m_srcLoc};
    }
#if defined(COMPILER_MSVC)
#pragma warning(pop)
#endif

  private:
    template <class FormatStreamable>
    friend Throw& operator<<(Throw& r, const FormatStreamable& it)
    {
        r.m_stream << it;
        return r;
    }

    template <class FormatStreamable>
    friend Throw&& operator<<(Throw&& r, const FormatStreamable& it)
    {
        r.m_stream << it;
        return std::move(r);
    }

    std::stringstream m_stream{};
    source_location m_srcLoc{};
};

template <class BaseException>
class Exception : public BaseException
{
  public:
    using BaseException::BaseException;

    explicit Exception(const char* msg,
                       source_location srcLoc = source_location::current())
        : BaseException{msg}
        , m_srcLoc{srcLoc}
    {
    }

    source_location sourceLocation() const noexcept
    {
        return m_srcLoc;
    }

  private:
    source_location m_srcLoc{};
};

struct message
{
    message(std::string s, source_location sl)
        : loc{sl}
        , m{std::move(s)}
    {
    }

    [[nodiscard]] const std::string& msg() const
    {
        return m;
    }

    [[nodiscard]] source_location location() const
    {
        return loc;
    }

    std::string m{};
    source_location loc;
};

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
[[nodiscard]] std::string msg_from_ostringstream(const A&... args)
{
    std::ostringstream str{};
    return stream_args_into(str, args...).str();
}

template <class... Args>
struct [[maybe_unused]] intent
{
    explicit intent(source_location sl, Args... args)
        : tup{std::move(args)...}
        , loc{sl}
    {
    }

    ~intent()
    {
        if (std::uncaught_exceptions() <= except_count)
            return;
        try
        {
            msgs().push_back(std::apply(
                [this](const auto&... args) {
                    return message{msg_from_ostringstream(args...), loc};
                },
                tup));
        }
        catch (...)
        {
        }
    }

    std::tuple<Args...> tup;
    int except_count{std::uncaught_exceptions()};
    source_location loc;
};

using Error = Exception<std::runtime_error>;

int a(int i)
{
    const auto& in{intent{CURRENT_LOC, "a(", i, ')'}};
    if (i % 2 != 0)
        Throw<Error>{CURRENT_LOC} << "odd number not allowed";
    return i;
}

std::pair<int, std::string> a_range(int min, int max)
{
    auto accu{int{}};
    auto msg{""s};
    try
    {
        const auto& i{intent{CURRENT_LOC, "a_range(", min, ", ", max, ")"}};
        for (int idx{min}; idx < max; ++idx)
        {
            const auto& ii{intent{CURRENT_LOC, "accu:", accu, " idx:", idx}};
            accu += a(idx);
        }
    }
    catch (...)
    {
        msg += std::accumulate(msgs().cbegin(),
                               msgs().cend(),
                               ""s,
                               [](auto accu, const auto& m) {
                                   accu += m.msg() + '\n';
                                   return accu;
                               });
        msgs().clear();
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
    const auto& i{intent{CURRENT_LOC, "start", 0}};
    a(0);
    BOOST_TEST(msgs().empty());
}

BOOST_AUTO_TEST_CASE(intent_on_failure)
{
    const auto loc{CURRENT_LOC};
    try
    {
        const auto& i{intent{loc, "test a(", -1, ')'}};
        a(-1);
    }
    catch (...)
    {
    }
    BOOST_REQUIRE(!msgs().empty());
    BOOST_REQUIRE(msgs().size() == 2);
    BOOST_TEST(msgs()[0].msg() == "a(-1)");
    BOOST_TEST(msgs()[1].msg() == "test a(-1)");
    BOOST_TEST(msgs()[1].location().file_name() == loc.file_name());
    BOOST_TEST(msgs()[1].location().line() == loc.line());
    BOOST_TEST(msgs()[1].location().column() == loc.column());
    BOOST_TEST(msgs()[1].location().function_name() == loc.function_name());
}

BOOST_AUTO_TEST_CASE(intent_copies_lvalue)
{
    auto calls{""s};
    auto spy{cm_spy{&calls}};

    try
    {
        const auto i{intent{CURRENT_LOC, spy}};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "ccmc");
    BOOST_TEST(!msgs().empty());
}

BOOST_AUTO_TEST_CASE(intent_moves_rvalue)
{
    auto calls{""s};

    try
    {
        const auto i{intent{CURRENT_LOC, cm_spy{&calls}}};
        throw 42;
    }
    catch (...)
    {
    }

    BOOST_TEST(calls == "mc");
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
