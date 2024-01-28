#pragma once

#include <cstdint>
#include <ostream>

namespace xzr::ext
{
struct source_location
{
    [[nodiscard]] static constexpr source_location current(
        const char* const f = __builtin_FILE(),
        const std::uint_least32_t l = __builtin_LINE(),
#if defined(__clang__)
        const std::uint_least32_t c = __builtin_COLUMN(),
#else
        const std::uint_least32_t c = 0,
#endif
        const char* const func = __builtin_FUNCTION()) noexcept
    {
        source_location res{};
        res.lin = l;
        res.col = c;
        res.fil = f;
        res.funcname = func;
        return res;
    }

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

std::ostream& operator<<(std::ostream& s, const source_location& l);
}
