#include "string_helpers.h"

#include <algorithm>

namespace Util {

std::string string_lower(std::string src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return src;
}

std::string string_upper(std::string src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    return src;
}

} // namespace Util
