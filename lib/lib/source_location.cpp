#include <lib/source_location.hpp>

namespace xzr::ext
{
std::ostream& operator<<(std::ostream& s, const source_location& l)
{
    s << l.file_name() << ':' << l.line() << ':' << l.column() << ':'
      << l.function_name() << ':';
    return s;
}
}
