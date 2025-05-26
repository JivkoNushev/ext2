#pragma once

#include "Block.h"

class BlockGroupDescriptor : public Block
{
public:
// ---------------- PUBLIC TYPES ----------------
    struct Fields
    {
        uint32_t bg_block_bitmap = 0;
        uint32_t bg_inode_bitmap = 0;
        uint32_t bg_inode_table = 0;
        uint16_t bg_free_blocks_count = 0;
        uint16_t bg_free_inodes_count = 0;
        uint16_t bg_used_dirs_count = 0;
        uint16_t bg_pad = 0;
        uint8_t  bg_reserved[12]{};
    };

// ---------------- PUBLIC VARIABLES ----------------
    Fields m_fields;

// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    BlockGroupDescriptor();
    BlockGroupDescriptor(uint32_t size, uint32_t offset);
    ~BlockGroupDescriptor() = default;

// ---------------- PUBLIC METHODS ----------------
    void print_fields() const;

protected:
// ---------------- PROTECTED METHODS ----------------
    void* get_fields_buffer_for_read() override;
    const void* get_fields_buffer_for_write() const override;
};
