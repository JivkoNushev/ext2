#include <optional>

#include "utils.h"

namespace utils
{

int find_first_free_bit(uint8_t* bitmap, size_t size)
{
    int free_bit = -1;
    for (uint32_t byte_index = 0; byte_index < size; byte_index++) {
        if (bitmap[byte_index] == 0xFF) continue;

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (!(bitmap[byte_index] & (1 << bit))) return byte_index * 8 + bit;
        }
    }

    return free_bit;
}

std::vector<utils::string> split_path(const utils::string& path)
{
    std::vector<utils::string> result;
    size_t start = 0;
    while (true)
    {
        std::optional<size_t> pos = path.find('/', start);
        if (!pos)
        {
            if (start < path.size())
            {
                result.push_back(path.substr(start));
            }
            break;
        }
        if (*pos > start)
        {
            result.push_back(path.substr(start, *pos - start));
        }
        start = *pos + 1;
    }

    return result;
}

}
