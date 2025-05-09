#include <fstream>
#include <iostream>

#include "SuperBlock.h"


SuperBlock::SuperBlock()
{}

void SuperBlock::read(const char* file_name)
{
    std::ifstream ifs(file_name);
    ifs.read((char*)&this->fields, sizeof(this->fields));
}

uint32_t SuperBlock::get_inodes_count() const
{
    return this->fields.s_inodes_count;
}

uint32_t SuperBlock::get_blocks_count() const
{
    return this->fields.s_blocks_count;
}

void SuperBlock::print_fields() const
{
    std::cout << "s_inodes_count: "         << this->fields.s_inodes_count << '\n';
    std::cout << "s_blocks_count: "         << this->fields.s_blocks_count << '\n';
    std::cout << "s_r_blocks_count: "       << this->fields.s_r_blocks_count << '\n';
    std::cout << "s_free_blocks_count: "    << this->fields.s_free_blocks_count << '\n';
    std::cout << "s_free_inodes_count: "    << this->fields.s_free_inodes_count << '\n';
}
