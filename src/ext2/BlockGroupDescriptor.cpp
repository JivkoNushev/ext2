#include <fstream>
#include <iostream>

#include "BlockGroupDescriptor.h"

BlockGroupDescriptor::BlockGroupDescriptor() :
    Block(0, 0)
{}

BlockGroupDescriptor::BlockGroupDescriptor(uint32_t size, uint32_t offset) :
    Block(size, offset)
{}

uint32_t BlockGroupDescriptor::read(const char* file_name) 
{
    std::ifstream ifs(file_name);
    ifs.seekg(this->m_offset, std::ios_base::beg);
    ifs.read((char*)&this->fields, sizeof(this->fields));

    return sizeof(this->fields);
}

uint32_t BlockGroupDescriptor::write(const char* file) const
{
    (void)file;

    //TODO: Implement
    return 0;
}

void BlockGroupDescriptor::print_fields() const
{
    std::cout << "bg_block_bitmap: "            << this->fields.bg_block_bitmap << '\n';
    std::cout << "bg_inode_bitmap: "            << this->fields.bg_inode_bitmap << '\n';
    std::cout << "bg_inode_table: "             << this->fields.bg_inode_table << '\n';
    std::cout << "bg_free_blocks_count: "       << this->fields.bg_free_blocks_count << '\n';
    std::cout << "bg_free_inodes_count: "       << this->fields.bg_free_inodes_count << '\n';
    std::cout << "bg_used_dirs_count: "         << this->fields.bg_used_dirs_count << '\n';
    std::cout << "bg_pad: "                     << this->fields.bg_pad << '\n';
    std::cout << "bg_reserved: "                << this->fields.bg_reserved << '\n';
}

