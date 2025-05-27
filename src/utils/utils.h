#pragma once

#include <vector>

#include "string.h"
#include "cstring.h"

namespace utils
{
    int find_first_free_bit(uint8_t* bitmap, size_t size);

    std::vector<utils::string> split_path(const utils::string& path);
}
