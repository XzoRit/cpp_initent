#pragma once

#include <algorithm>
#include <cstdint>
#include <sstream>

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
}

#include <algorithm>
#include <cstdint>
#include <format>

namespace std
{
template <>
struct formatter<::xzr::ext::source_location, char>
{
    enum spec : std::uint8_t
    {
        none,
        file,
        line,
        col,
        func
    };

    struct spec_table_enty
    {
        char c{};
        spec s{none};
    };

    static constexpr spec_table_enty spec_table[4]{{'F', file},
                                                   {'L', line},
                                                   {'C', col},
                                                   {'f', func}};

    spec specfier{none};

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it{ctx.begin()};
        if (it == ctx.end())
            return it;

        if (auto mod{std::ranges::find(spec_table, *it, &spec_table_enty::c)};
            mod == std::end(spec_table))
            throw std::format_error{"invalid format for source_location "
                                    "missing one of F, L, C or f"};
        else
            specfier = mod->s;

        if (*(++it) != '}')
            throw std::format_error{"invalid format for source_location "
                                    "only one specifier allowed"};
        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(::xzr::ext::source_location sl,
                                FmtContext& ctx) const
    {
        auto&& out{ctx.out()};

        if (specfier == none)
            return out;

        if (specfier == file)
            std::format_to(out, "{}", sl.file_name());
        else if (specfier == line)
            std::format_to(out, "{}", sl.line());
        else if (specfier == col)
            std::format_to(out, "{}", sl.column());
        else if (specfier == func)
            std::format_to(out, "{}", sl.function_name());

        return out;
    }
};
}
