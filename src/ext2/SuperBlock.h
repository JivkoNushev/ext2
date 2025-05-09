#pragma once

#include <cstdint>

class SuperBlock
{
public:
    SuperBlock();

    void read(const char* file_name);

    uint32_t get_inodes_count() const;
    uint32_t get_blocks_count() const;


    void print_fields() const;
private:
    struct SuperBlockDiskFields
    {
        uint32_t s_inodes_count = 0;
        uint32_t s_blocks_count = 0;
        uint32_t s_r_blocks_count = 0;
        uint32_t s_free_blocks_count = 0;
        uint32_t s_free_inodes_count = 0;
    };

    SuperBlockDiskFields fields;
};
