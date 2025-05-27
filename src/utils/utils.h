#pragma once

#include "string.h"
#include "cstring.h"
#include "vector.h"

namespace utils
{
    int find_first_free_bit(uint8_t* bitmap, size_t size);

    utils::vector<utils::string> split_path(const utils::string& path);
}
