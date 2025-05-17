#pragma once

#include <cstdint>

#include "Block.h"

class BlockGroupDescriptor : public Block
{
public:
    BlockGroupDescriptor();
    BlockGroupDescriptor(uint32_t size, uint32_t offset);
    ~BlockGroupDescriptor() = default;

    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    void print_fields() const;

    uint32_t get_inode_table() const;

public:
    static constexpr const uint16_t GD_SIZE = 32;

private:
    struct BlockGroupDescriptorFields
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

private:
    BlockGroupDescriptorFields m_fields;
};
