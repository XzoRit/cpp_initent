#pragma once

#include <cstdint>
#include <ostream>

#if __has_include(<source_location>)
#include <source_location>
#endif

namespace xzr::ext
{
#if defined(__cpp_lib_source_location)
using source_location = std::source_location;
#elif __has_builtin(__builtin_source_location)
struct source_location
{
  private:
    struct impl_t
    {
        const char* fil;
        const char* funcname;
        std::uint_least32_t lin;
        std::uint_least32_t col;
    };
    using builtin_type = decltype(__builtin_source_location());

  public:
    [[nodiscard]] static constexpr source_location current(
        builtin_type slptr = __builtin_source_location()) noexcept
    {
        source_location ret;
        ret.impl = static_cast<const impl_t*>(slptr);
        return ret;
    }

    [[nodiscard]] constexpr source_location() noexcept
    {
    }

    [[nodiscard]] constexpr std::uint_least32_t line() const noexcept
    {
        return impl ? impl->lin : 0u;
    }

    [[nodiscard]] constexpr std::uint_least32_t column() const noexcept
    {
        return impl ? impl->col : 0u;
    }

    [[nodiscard]] constexpr const char* file_name() const noexcept
    {
        return impl ? impl->fil : "";
    }

    [[nodiscard]] constexpr const char* function_name() const noexcept
    {
        return impl ? impl->funcname : "";
    }

  private:
    const impl_t* impl{nullptr};
};
#else
struct source_location
{
#if (__has_builtin(__builtin_FILE) && __has_builtin(__builtin_LINE) &&         \
     __has_builtin(__builtin_COLUMN) && __has_builtin(__builtin_FUNCSIG))
    [[nodiscard]] static constexpr source_location current(
        const char* const f = __builtin_FILE(),
        const std::uint_least32_t l = __builtin_LINE(),
        const std::uint_least32_t c = __builtin_COLUMN(),
        const char* const func = __builtin_FUNCSIG()) noexcept
    {
        source_location res{};
        res.lin = l;
        res.col = c;
        res.fil = f;
        res.funcname = func;
        return res;
    }
#elif (__has_builtin(__builtin_FILE) && __has_builtin(__builtin_LINE) &&       \
       __has_builtin(__builtin_COLUMN) && __has_builtin(__builtin_FUNCTION))
    [[nodiscard]] static constexpr source_location current(
        const char* const f = __builtin_FILE(),
        const std::uint_least32_t l = __builtin_LINE(),
        const std::uint_least32_t c = __builtin_COLUMN(),
        const char* const func = __builtin_FUNCTION()) noexcept
    {
        source_location res{};
        res.lin = l;
        res.col = c;
        res.fil = f;
        res.funcname = func;
        return res;
    }
#else
    [[nodiscard]] static constexpr source_location current(
        const char* const f,
        const std::uint_least32_t l,
        const std::uint_least32_t c,
        const char* const func) noexcept
    {
        source_location res{};
        res.lin = l;
        res.col = c;
        res.fil = f;
        res.funcname = func;
        return res;
    }

    [[nodiscard]] static constexpr source_location current() noexcept
    {
        return {};
    }
#if defined(_MSC_VER) && !defined(__clang__)
    ::xzr::ext::source_location::current(__FILE__, __LINE__, 0, __FUNCSIG__)
#else
    ::xzr::ext::source_location::current(__FILE__,
                                         __LINE__,
                                         0,
                                         __PRETTY_FUNCTION__)
#endif
#endif

    [[nodiscard]] constexpr source_location() noexcept = default;

    [[nodiscard]] constexpr std::uint_least32_t line() const noexcept
    {
        return lin;
    }

    [[nodiscard]] constexpr std::uint_least32_t column() const noexcept
    {
        return col;
    }

    [[nodiscard]] constexpr const char* file_name() const noexcept
    {
        return fil;
    }

    [[nodiscard]] constexpr const char* function_name() const noexcept
    {
        return funcname;
    }

  private:
    std::uint_least32_t lin{};
    std::uint_least32_t col{};
    const char* fil = "unknown_file";
    const char* funcname = "unknown_func";
};
#endif

std::ostream& operator<<(std::ostream& s, const source_location& l);
}
