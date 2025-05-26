#pragma once

#include <cstdint>

#include "Block.h"

class SuperBlock : public Block
{
public:
// ---------------- PUBLIC CONSTANTS ----------------
    static constexpr const uint16_t SB_OFFSET = 1024;
    static constexpr const uint16_t SB_SIZE = 1024;

// ---------------- PUBLIC TYPES ----------------
    struct Fields
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
        // EXT2_DYNAMIC_REV Specific
        uint32_t s_first_ino = 0;
        uint16_t s_inode_size = 0;
        uint16_t s_block_group_nr = 0;
        uint32_t s_feature_compat = 0;
        uint32_t s_feature_incompat = 0;
        uint32_t s_feature_ro_compat = 0;
        uint8_t  s_uuid[16]{};
        uint8_t  s_volume_name[16]{};
        uint8_t  s_last_mounted[64]{};
        uint32_t s_algo_bitmap = 0;
        // Performance Hints
        uint8_t  s_prealloc_blocks = 0;
        uint8_t  s_prealloc_dir_blocks = 0;
        uint8_t  _s_perf_hints_allignment[2]{};
        // Journaling Support
        uint8_t  s_journal_uuid[16]{};
        uint32_t s_journal_inum = 0;
        uint32_t s_journal_dev = 0;
        uint32_t s_last_orphan = 0;
        // Directory Indexing Support
        uint32_t s_hash_seed[4]{};
        uint8_t  s_def_hash_version = 0;
        uint8_t  _s_dir_ind_sup_allignment[3]{};
        // Other options
        uint32_t s_default_mount_options = 0;
        uint32_t s_first_meta_bg = 0;
        uint8_t  _s_reserved_for_future_rev[760]{};
    };

// ---------------- PUBLIC VARIABLES ----------------
    Fields m_fields;

// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    SuperBlock();
    SuperBlock(uint32_t size, uint32_t offset);
    SuperBlock(uint32_t size, uint32_t offset, const char* device_path);
    ~SuperBlock() = default;
 
// ---------------- PUBLIC METHODS ----------------
    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    uint16_t get_bg_count() const;
    uint32_t get_block_size() const;

    void print_fields() const;

private:
// ---------------- PRIVATE VARIABLES ----------------
    uint16_t m_block_group_count = 0;
    uint32_t m_block_size = 0;
};
