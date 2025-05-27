#include "cstring.h"

void utils::memcpy(char* dst, const char* src, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        dst[i] = src[i];
    }
}

void utils::strcpy(char* dst, const char* src)
{
    while(*src)
    {
        *dst = *src;
        dst++;
        src++;
    }

    *dst = '\0';
}

size_t utils::strlen(const char* str)
{
    if(!str) return 0;

    size_t len = 0;
    while(*str)
    {
        len++;
        str++;
    }

    return len;
}

void utils::strcat(char* dst, const char* src)
{
    while(*dst) dst++;
    utils::strcpy(dst, src);
}

int utils::strcmp(const char* l, const char* r)
{
    while(*l && (*l == *r))
    {
        l++;
        r++;
    }

    return *l - *r;
}

int utils::strncmp(const char* l, const char* r, size_t n)
{
    while (n-- && *l && (*l == *r))
    {
        ++l;
        ++r;
    }

    if (n == (size_t)-1) return 0;

    return *l - *r;
}

char* utils::strchr(char* str, char c)
{
    while(*str)
    {
        if(*str == c) return str;
        str++;
    }

    return c == '\0' ? str : nullptr;
}
