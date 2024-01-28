#pragma once

#include <lib/source_location.hpp>

#include <sstream>
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

#if defined(_MSVC_VER)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~thrw() noexcept(false)
    {
        m_ex.str().append(m_stream.str());
        throw m_ex;
    }
#if defined(_MSVC_VER)
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
}
