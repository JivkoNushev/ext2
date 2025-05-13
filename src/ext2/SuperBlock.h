#pragma once

#include <cstdint>

#include "Block.h"

class SuperBlock : public Block
{
public:
    SuperBlock();
    SuperBlock(uint32_t size, uint32_t offset);
    ~SuperBlock() = default;
 
    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    void load();

    uint16_t get_bg_count() const;

    uint32_t get_inodes_count() const;
    uint32_t get_blocks_count() const;

    void print_fields() const;

public:
    static constexpr const uint16_t SB_OFFSET = 1024;
    static constexpr const uint16_t SB_SIZE = 1024;

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

private:

    SuperBlockFields m_fields;

    uint16_t m_bg_count = 0;
};
