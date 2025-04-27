#pragma once

#include <cstdint>

class BlockGroupDescriptorTable
{
public:

private:
    uint32_t m_block_bitmap = 0;
    uint32_t m_inode_bitmap = 0;
    uint32_t m_inode_table = 0;

    uint16_t m_free_blocks_count = 0;
    uint16_t m_free_inodes_count = 0;
    uint16_t m_used_dirs_count = 0;
};
