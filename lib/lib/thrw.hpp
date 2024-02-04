#pragma once

#include <lib/source_location.hpp>

#include <sstream>
#include <type_traits>
#include <utility>

namespace xzr::error
{
template <class Exception>
class [[maybe_unused]] thrw
{
  private:
    using source_location = ::xzr::ext::source_location;

  public:
    template <class... Args>
    explicit thrw(source_location sl, Args... args)
        : m_ex{std::move(args)..., sl}
    {
    }

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~thrw() noexcept(false)
    {
        m_ex.str().append(m_stream.str());
        throw m_ex;
    }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

  private:
    template <class FormatStreamable>
    friend thrw& operator<<(thrw& r, const FormatStreamable& it)
    {
        r.m_stream << it;
        return r;
    }

    template <class FormatStreamable>
    friend thrw&& operator<<(thrw&& r, const FormatStreamable& it)
    {
        r.m_stream << it;
        return std::move(r);
    }

    source_location m_srcLoc{};
    std::stringstream m_stream{};
    Exception m_ex;
};

template <class E>
struct collect
{
    template <class... Args>
    auto operator()(Args... args)
    {
        return thrw<E>{m_sl, args...};
    }

    ::xzr::ext::source_location m_sl{};
};

template <class E>
auto throw_it(
    ::xzr::ext::source_location sl = ::xzr::ext::source_location::current())
{
    return collect<E>{sl};
}

template <class E>
class [[maybe_unused]] thrw_msg
{
  private:
    using source_location = ::xzr::ext::source_location;

  public:
    thrw_msg() = default;
    explicit thrw_msg(const char* msg)
    {
        m_stream << msg;
    }
    explicit thrw_msg(const char* msg, source_location sl)
        : m_sl{sl}
    {
        m_stream << msg;
    }
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~thrw_msg() noexcept(false)
    {
        if constexpr (std::is_constructible_v<E, const char*, source_location>)
        {
            throw E{m_stream.str().c_str(), m_sl};
        }
        else
        {
            throw E{m_stream.str().c_str()};
        }
    }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

  private:
    template <class FormatStreamable>
    friend thrw_msg&& operator<<(thrw_msg&& r, const FormatStreamable& it)
    {
        r.m_stream << it;
        return std::move(r);
    }

    std::stringstream m_stream{};
    source_location m_sl;
};

template <class E>
auto throw_msg()
{
    return thrw_msg<E>{};
}

template <class E>
auto throw_msg(
    const char* msg,
    ::xzr::ext::source_location sl = ::xzr::ext::source_location::current())
{
    if constexpr (std::is_constructible_v<E,
                                          const char*,
                                          ::xzr::ext::source_location>)
    {
        return thrw_msg<E>{msg, sl};
    }
    else
    {
        return thrw_msg<E>{msg};
    }
}
}
