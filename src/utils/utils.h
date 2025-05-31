#pragma once

#include "string.h"
#include "cstring.h"
#include "vector.h"

namespace utils
{
    utils::vector<utils::string> split_str(const utils::string& path, char delimiter);

    utils::vector<utils::string> split_path(const utils::string& path);

    utils::vector<utils::string> split_words(const utils::string& line);

    utils::string get_entry_name(const utils::string& path);

    utils::string get_parent_path(const utils::string& path);

    bool is_bit_set(uint8_t* bitmap, int bit);

    bool is_bit_clear(uint8_t* bitmap, int bit);

    int find_first_free_bit(uint8_t* bitmap, size_t size);

    void set_bit(uint8_t* bitmap, int bit);
}
