#include <iostream>

#include "BlockGroupDescriptor.h"

BlockGroupDescriptor::BlockGroupDescriptor() :
    Block(0, 0)
{}

BlockGroupDescriptor::BlockGroupDescriptor(uint32_t size, uint32_t offset) :
    Block(size, offset)
{}

void BlockGroupDescriptor::print_fields() const
{
    std::cout << "bg_block_bitmap: "            << this->m_fields.bg_block_bitmap << '\n';
    std::cout << "bg_inode_bitmap: "            << this->m_fields.bg_inode_bitmap << '\n';
    std::cout << "bg_inode_table: "             << this->m_fields.bg_inode_table << '\n';
    std::cout << "bg_free_blocks_count: "       << this->m_fields.bg_free_blocks_count << '\n';
    std::cout << "bg_free_inodes_count: "       << this->m_fields.bg_free_inodes_count << '\n';
    std::cout << "bg_used_dirs_count: "         << this->m_fields.bg_used_dirs_count << '\n';
    std::cout << "bg_pad: "                     << this->m_fields.bg_pad << '\n';
    std::cout << "bg_reserved: "                << this->m_fields.bg_reserved << '\n';
}

void* BlockGroupDescriptor::get_fields_buffer_for_read()
{
    return &this->m_fields;
}

const void* BlockGroupDescriptor::get_fields_buffer_for_write() const
{
    return &this->m_fields;
}
