#pragma once

#include <stdint.h>

struct DirectoryEntry {};

struct LinkedDirectoryEntry : public DirectoryEntry
{
    uint32_t inode = 0;
    uint16_t rec_len = 0;
    uint8_t  name_len = 0;
    uint8_t  file_type = 0;
    uint8_t  name[255];
};
