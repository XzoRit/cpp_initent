#pragma once

#include <lib/source_location.hpp>

#include <sstream>
#include <utility>

namespace xzr::error
{
template <class Exception>
class [[maybe_unused]] thrw
{
  public:
    explicit thrw(ext::source_location sl = ext::source_location::current())
        : m_srcLoc{sl}
    {
    }

#if defined(DFL_CONFIG_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
    [[noreturn]] ~thrw() noexcept(false)
    {
        throw Exception{m_stream.str().c_str(), m_srcLoc};
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

    std::stringstream m_stream{};
    ext::source_location m_srcLoc;
};
}
