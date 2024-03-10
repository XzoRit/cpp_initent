#pragma once

#include <lib/source_location.hpp>

#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace xzr::error
{
struct message
{
    using source_location = ::xzr::ext::source_location;

    message(std::string s, source_location sl = source_location::current())
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
    source_location loc{source_location::current()};
};

using messages = std::vector<message>;

inline messages& msgs()
{
    thread_local static messages ms{};
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
}