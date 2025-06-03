#include "utils.h"

utils::vector<utils::string> utils::split_str(const utils::string& path, char delimiter)
{
    utils::vector<utils::string> result;
    size_t start = 0;

    while (start < path.size())
    {
        while (start < path.size() && path[start] == delimiter)
            ++start;

        if (start >= path.size())
            break;

        size_t end = start;
        while (end < path.size() && path[end] != delimiter)
            ++end;

        result.push_back(path.substr(start, end - start));

        start = end;
    }

    return result;
}

utils::vector<utils::string> utils::split_path(const utils::string& path)
{
    return utils::split_str(path, '/');
}

utils::vector<utils::string> utils::split_words(const utils::string& line)
{
    return utils::split_str(line, ' ');
}

utils::string utils::get_entry_name(const utils::string& path)
{
    std::optional<size_t> last_slash_pos = path.find_last_of('/');
    if (!last_slash_pos) return path;

    return path.substr(*last_slash_pos + 1);
}

utils::string utils::get_parent_path(const utils::string& path)
{
    std::optional<size_t> last_slash_pos = path.find_last_of('/');
    if (!last_slash_pos) return "/";

    return path.substr(0, *last_slash_pos);
}

bool utils::is_bit_set(uint8_t* bitmap, int bit)
{
    return bitmap[bit / 8] & 1 << bit % 8;
}

bool utils::is_bit_clear(uint8_t* bitmap, int bit)
{
    return !utils::is_bit_set(bitmap, bit);
}

int utils::find_first_free_bit(uint8_t* bitmap, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        for(size_t j = 0; j < 8; j++)
        {
            int bit = i * 8 + j;
            if(utils::is_bit_clear(bitmap, bit)) return bit;
        } 
    }

    return -1;
}

void utils::set_bit(uint8_t* bitmap, int bit)
{
    bitmap[bit / 8] |= 1 << bit % 8;
}

void utils::clear_bit(uint8_t* bitmap, int bit)
{
    bitmap[bit / 8] &= ~(1 << bit % 8);
}

void utils::print_utf8(utils::vector<uint8_t> data)
{
    std::cout.write((const char*)data.raw(), data.size());
}
