#pragma once

#include <stdint.h>

struct DirectoryEntry
{
public:
    enum FileType : uint8_t
    {
        EXT2_FT_UNKNOWN     = 0, // Unknown File Type
        EXT2_FT_REG_FILE    = 1, // Regular File
        EXT2_FT_DIR         = 2, // Directory File
        EXT2_FT_CHRDEV      = 3, // Character Device
        EXT2_FT_BLKDEV      = 4, // Block Device
        EXT2_FT_FIFO        = 5, // Buffer File
        EXT2_FT_SOCK        = 6, // Socket File
        EXT2_FT_SYMLINK     = 7  // Symbolic Link
    };

};

struct LinkedDirectoryEntry : public DirectoryEntry
{
    uint32_t inode = 0;
    uint16_t rec_len = 0;
    uint8_t  name_len = 0;
    uint8_t  file_type = 0;
    uint8_t  name[255];
};
