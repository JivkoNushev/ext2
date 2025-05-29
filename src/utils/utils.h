#pragma once

#include "string.h"
#include "cstring.h"
#include "vector.h"

namespace utils
{
    utils::vector<utils::string> split_str(const utils::string& path, char delimiter);

    utils::vector<utils::string> split_path(const utils::string& path);

    utils::vector<utils::string> split_words(const utils::string& line);
}
