#pragma once

#include "Block.h"
#include "Disk.h"


class SuperBlock : public Block
{
public:
    SuperBlock(uint32_t offset = SUPERBLOCK_OFFSET);

    void load(Disk& disk);
    void save(Disk& disk) const;
    void format(uint32_t blocks_per_group = 8192, uint32_t inodes_per_group = 2048, uint32_t disk_size = 1 << 24);

private:
    struct __attribute__((packed)) SuperBlockFields
    {
        uint32_t s_inodes_count = 0;
        uint32_t s_blocks_count = 0;
        uint32_t s_r_blocks_count = 0;
        uint32_t s_free_blocks_count = 0;
        uint32_t s_free_inodes_count = 0;
        uint32_t s_first_data_block = 0;
        uint32_t s_log_block_size = 0;
        uint32_t s_log_frag_size = 0;
        uint32_t s_blocks_per_group = 0;
        uint32_t s_frags_per_group = 0;
        uint32_t s_inodes_per_group = 0;
        uint32_t s_mtime = 0;
        uint32_t s_wtime = 0;
        uint16_t s_mnt_count = 0;
        uint16_t s_max_mnt_count = 0;
        uint16_t s_magic = 0;
        uint16_t s_state = 0;
        uint16_t s_errors = 0;
        uint16_t s_minor_rev_level = 0;
        uint32_t s_lastcheck = 0;
        uint32_t s_checkinterval = 0;
        uint32_t s_creator_os = 0;
        uint32_t s_rev_level = 0;
        uint16_t s_def_resuid = 0;
        uint16_t s_def_resgid = 0;

        uint8_t padding[1024 - 84];
    };

    struct __attribute__((packed)) SuperBlockType
    {
        SuperBlockFields fields;
    };

    static const uint16_t MAGIC_BYTES = 0xEF53;

    SuperBlockFields fields;
};
