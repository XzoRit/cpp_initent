#pragma once

#include <lib/source_location.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace xzr::error
{
struct intention_entry
{
    virtual ~intention_entry() = default;
    virtual void stream_into(std::ostream&) = 0;
};

using intention_ptrs = std::vector<std::unique_ptr<intention_entry>>;

inline intention_ptrs& intention_container()
{
    thread_local static intention_ptrs iptrs{};
    return iptrs;
}

inline void dump_intents(std::ostream& s)
{
    for (auto&& i : intention_container())
    {
        i->stream_into(s);
        s << '\n';
    }
}

struct intention_stack_entry
{
    using source_location = ::xzr::ext::source_location;

    intention_stack_entry(source_location sl, std::string s)
        : loc{sl}
        , m{std::move(s)}
    {
    }

    [[nodiscard]] const std::string& msg() const
    {
        return m;
    }

    [[nodiscard]] source_location where() const
    {
        return loc;
    }

    source_location loc{source_location::current()};
    std::string m{};
};

using intentions = std::vector<intention_stack_entry>;

inline intentions& intention_stack()
{
    thread_local static intentions ms{};
    return ms;
}

template <class Stream, class A>
inline void stream_arg_into(Stream& str, const A& arg)
{
    str << arg;
}

template <class Stream, class A, class... Args>
inline Stream& stream_args_into(Stream& str, const A& arg, const Args&... args)
{
    stream_arg_into(str, arg);
    ([&] { str << ' ' << args; }(), ...);
    return str;
}

template <class... A>
[[nodiscard]] inline std::string msg_from_ostringstream(const A&... args)
{
    std::ostringstream str{};
    return stream_args_into(str, args...).str();
}

template <class... Args>
struct [[nodiscard]] intent_t
{
    using source_location = ::xzr::ext::source_location;

    struct entry final : intention_entry
    {
        explicit entry(std::tuple<Args...>&& t)
            : tup{std::move(t)}
        {
        }

        void stream_into(std::ostream& s) override
        {
            std::apply(
                [&s](const auto&... args) { stream_args_into(s, args...); },
                tup);
        }

        std::tuple<Args...> tup;
    };

    explicit intent_t(source_location sl, Args... args)
        : tup{std::move(args)...}
        , loc{sl}
    {
    }

    ~intent_t()
    {
        if (std::uncaught_exceptions() <= except_count)
            return;
        try
        {
            intention_stack().push_back(std::apply(
                [this](const auto&... args) {
                    return intention_stack_entry{
                        loc,
                        msg_from_ostringstream(args...)};
                },
                tup));
            intention_container().push_back(
                std::make_unique<entry>(std::move(tup)));
        }
        catch (...)
        {
        }
    }

    std::tuple<Args...> tup;
    int except_count{std::uncaught_exceptions()};
    source_location loc{source_location::current()};
};

struct capture_t
{
    auto capture() const&& = delete;

    template <class... Args>
    auto capture(Args&&... args) const&&
    {
        return intent_t{sl, std::forward<Args>(args)...};
    }

    using source_location = ::xzr::ext::source_location;
    source_location sl{};
};

inline auto intent(
    ::xzr::ext::source_location sl = ::xzr::ext::source_location::current())
{
    return capture_t{sl};
}

struct format_func_t
{
    using source_location = ::xzr::ext::source_location;

    template <class ExitFunc>
    struct push_intent_on_fail
    {
        push_intent_on_fail(ExitFunc ef, source_location sl)
            : m_ef{std::move(ef)}
            , m_sl{sl}
        {
        }

        ~push_intent_on_fail()
        {
            if (std::uncaught_exceptions() <= except_count)
                return;
            try
            {
                intention_stack().push_back({m_sl, m_ef()});
            }
            catch (...)
            {
            }
        }

        int except_count{std::uncaught_exceptions()};
        ExitFunc m_ef;
        source_location m_sl;
    };

    template <class Func>
    auto on_fail_msg(Func&& func) const&&
    {
        return push_intent_on_fail{std::forward<Func>(func), m_sl};
    }

    source_location m_sl{};
};

inline auto intent_f(
    ::xzr::ext::source_location sl = ::xzr::ext::source_location::current())
{
    return format_func_t{sl};
}
}
