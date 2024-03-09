#pragma once

#include <lib/source_location.hpp>

#include <sstream>
#include <type_traits>
#include <utility>

namespace xzr::error
{
template <class Exception>
class [[maybe_unused]] raise_type
{
  private:
    using source_location = ::xzr::ext::source_location;

  public:
    explicit raise_type(source_location sl)
        : m_srcLoc{sl}
    {
    }

#if defined(_MSVC_VER)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~raise_type() noexcept(false)
    {
        if constexpr (std::is_constructible_v<Exception,
                                              std::string,
                                              source_location>)
        {
            throw Exception{m_stream.str(), m_srcLoc};
        }
        else if constexpr (std::is_constructible_v<Exception,
                                                   const char*,
                                                   source_location>)
        {
            throw Exception{m_stream.str().c_str(), m_srcLoc};
        }
        else
        {
            throw Exception{m_stream.str().c_str()};
        }
    }
#if defined(_MSVC_VER)
#pragma warning(pop)
#endif

  private:
    template <class FormatStreamable>
    friend raise_type&& operator<<(raise_type&& r, const FormatStreamable& it)
    {
        r.m_stream << it;
        return std::move(r);
    }

    source_location m_srcLoc{};
    std::stringstream m_stream{};
};

template <class Exception, class Data>
class [[maybe_unused]] raise_type_t
{
  private:
    using source_location = ::xzr::ext::source_location;

  public:
    explicit raise_type_t(source_location sl, Data data)
        : m_srcLoc{sl}
        , m_data{std::move(data)}
    {
    }

#if defined(_MSVC_VER)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~raise_type_t() noexcept(false)
    {
        throw Exception{m_stream.str(), m_data, m_srcLoc};
    }
#if defined(_MSVC_VER)
#pragma warning(pop)
#endif

  private:
    template <class FormatStreamable>
    friend raise_type_t&& operator<<(raise_type_t&& r,
                                     const FormatStreamable& it)
    {
        r.m_stream << it;
        return std::move(r);
    }

    source_location m_srcLoc{};
    std::stringstream m_stream{};
    Data m_data;
};

template <class E>
auto raise(
    ::xzr::ext::source_location sl = ::xzr::ext::source_location::current());
template <class E, class Arg>
auto raise(
    Arg arg,
    ::xzr::ext::source_location sl = ::xzr::ext::source_location::current());

template <class E>
inline auto raise(::xzr::ext::source_location sl)
{
    return raise_type<E>{sl};
}

template <class E, class Arg>
inline auto raise(Arg arg, ::xzr::ext::source_location sl)
{
    return raise_type_t<E, Arg>{sl, arg};
}
}
