#pragma once

#include <cstddef>

namespace utils
{

    void memcpy(char* dst, const char* src, size_t size);

    void strcpy(char* dst, const char* src);

    size_t strlen(const char* str);

    void strcat(char* dst, const char* src);

    int strcmp(const char* l, const char* r);

    char* strchr(char* str, char c);

}
