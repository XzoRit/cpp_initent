#pragma once

#include <lib/source_location.hpp>

#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace xzr::error
{
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

    std::string m{};
    source_location loc{source_location::current()};
};

using intentions = std::vector<intention_stack_entry>;

inline intentions& intention_stack()
{
    thread_local static intentions ms{};
    return ms;
}

template <class Stream, class... A>
inline Stream& stream_args_into(Stream& str, const A&... args)
{
    (str << ... << args);
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
    template <class... Args>
    auto capture(Args&&... args) const&&
    {
        return intent_t{m_sl, std::forward<Args>(args)...};
    }

    using source_location = ::xzr::ext::source_location;
    source_location m_sl{};
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
