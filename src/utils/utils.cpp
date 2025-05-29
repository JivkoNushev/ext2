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
