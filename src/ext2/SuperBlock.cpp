#include <fstream>
#include <iostream>

#include "SuperBlock.h"


SuperBlock::SuperBlock() :
    Block(SuperBlock::SB_SIZE, SuperBlock::SB_OFFSET)
{}

uint32_t SuperBlock::read(const char* file_name) 
{
    std::ifstream ifs(file_name);
    ifs.seekg(this->SB_OFFSET, std::ios_base::beg);
    ifs.read((char*)&this->fields, sizeof(this->fields));

    return sizeof(this->fields);
}

uint32_t SuperBlock::write(const char* file) const
{
    (void)file;

    //TODO: Implement
    return 0;
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
    std::cout << "s_first_data_block: "     << this->fields.s_first_data_block << '\n';
    std::cout << "s_log_block_size: "       << this->fields.s_log_block_size << '\n';
    std::cout << "s_log_frag_size: "        << this->fields.s_log_frag_size << '\n';
    std::cout << "s_blocks_per_group: "     << this->fields.s_blocks_per_group << '\n';
    std::cout << "s_frags_per_group: "      << this->fields.s_frags_per_group << '\n';
    std::cout << "s_inodes_per_group: "     << this->fields.s_inodes_per_group << '\n';
    std::cout << "s_mtime: "                << this->fields.s_mtime << '\n';
    std::cout << "s_wtime: "                << this->fields.s_wtime << '\n';
    std::cout << "s_mnt_count: "            << this->fields.s_mnt_count << '\n';
    std::cout << "s_max_mnt_count: "        << this->fields.s_max_mnt_count << '\n';
    std::cout << "s_magic: "                << this->fields.s_magic << '\n';
    std::cout << "s_state: "                << this->fields.s_state << '\n';
    std::cout << "s_errors: "               << this->fields.s_errors << '\n';
    std::cout << "s_minor_rev_level: "      << this->fields.s_minor_rev_level << '\n';
    std::cout << "s_lastcheck: "            << this->fields.s_lastcheck << '\n';
    std::cout << "s_checkinterval: "        << this->fields.s_checkinterval << '\n';
    std::cout << "s_creator_os: "           << this->fields.s_creator_os << '\n';
    std::cout << "s_rev_level: "            << this->fields.s_rev_level << '\n';
    std::cout << "s_def_resuid: "           << this->fields.s_def_resuid << '\n';
    std::cout << "s_def_resgid: "           << this->fields.s_def_resgid << '\n';
}
