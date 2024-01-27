#pragma once

#include <lib/source_location.hpp>

#include <source_location>
#include <sstream>
#include <utility>

namespace xzr::error
{
template <class Exception>
class [[maybe_unused]] thrw
{
    std::source_location m_srcLoc = std::source_location::current();

  public:
    template <class... Args>
    explicit thrw(Args... args)
        : m_ex{std::move(args)..., m_srcLoc}
    {
    }

#if defined(DFL_CONFIG_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~thrw() noexcept(false)
    {
        throw m_ex;
    }
#if defined(COMPILER_MSVC)
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

    Exception m_ex;
    std::stringstream m_stream{};
};
}
