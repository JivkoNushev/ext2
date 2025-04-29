#pragma once

#include "Block.h"
#include "Disk.h"


class SuperBlock
{
public:
    SuperBlock(); // calls load

    void load(Disk& disk);
    void save(Disk& disk) const;
    void format(uint32_t total_blocks, uint32_t total_inodes, uint32_t block_size);

private:
    struct SuperBlockFields
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

    };

    static const uint16_t SUPERBLOCK_MAGIC_BYTES = 0xEF53;
    static const uint32_t SUPERBLOCK_OFFSET = 1024;

    SuperBlockFields fields;
};
