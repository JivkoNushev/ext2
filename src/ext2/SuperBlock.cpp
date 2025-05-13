#include <cstdint>
#include <fstream>
#include <iostream>

#include "SuperBlock.h"

SuperBlock::SuperBlock() :
    Block(SB_OFFSET, SB_SIZE)
{}

SuperBlock::SuperBlock(uint32_t size, uint32_t offset) :
    Block(size, offset)
{}

uint32_t SuperBlock::read(const char* file_name) 
{
    std::ifstream ifs(file_name);
    ifs.seekg(this->m_offset, std::ios_base::beg);
    ifs.read((char*)&this->m_fields, sizeof(this->m_fields));

    return sizeof(this->m_fields);
}

uint32_t SuperBlock::write(const char* file) const
{
    (void)file;

    //TODO: Implement
    return 0;
}

void SuperBlock::load()
{
    this->m_bg_count = this->m_fields.s_blocks_count / this->m_fields.s_blocks_per_group + (this->m_fields.s_blocks_count % this->m_fields.s_blocks_per_group != 0);
}

uint16_t SuperBlock::get_bg_count() const
{
    return this->m_bg_count;
}

uint32_t SuperBlock::get_inodes_count() const
{
    return this->m_fields.s_inodes_count;
}

uint32_t SuperBlock::get_blocks_count() const
{
    return this->m_fields.s_blocks_count;
}

void SuperBlock::print_fields() const 
{
    std::cout << "s_inodes_count: "         << this->m_fields.s_inodes_count << '\n';
    std::cout << "s_blocks_count: "         << this->m_fields.s_blocks_count << '\n';
    std::cout << "s_r_blocks_count: "       << this->m_fields.s_r_blocks_count << '\n';
    std::cout << "s_free_blocks_count: "    << this->m_fields.s_free_blocks_count << '\n';
    std::cout << "s_free_inodes_count: "    << this->m_fields.s_free_inodes_count << '\n';
    std::cout << "s_first_data_block: "     << this->m_fields.s_first_data_block << '\n';
    std::cout << "s_log_block_size: "       << this->m_fields.s_log_block_size << '\n';
    std::cout << "s_log_frag_size: "        << this->m_fields.s_log_frag_size << '\n';
    std::cout << "s_blocks_per_group: "     << this->m_fields.s_blocks_per_group << '\n';
    std::cout << "s_frags_per_group: "      << this->m_fields.s_frags_per_group << '\n';
    std::cout << "s_inodes_per_group: "     << this->m_fields.s_inodes_per_group << '\n';
    std::cout << "s_mtime: "                << this->m_fields.s_mtime << '\n';
    std::cout << "s_wtime: "                << this->m_fields.s_wtime << '\n';
    std::cout << "s_mnt_count: "            << this->m_fields.s_mnt_count << '\n';
    std::cout << "s_max_mnt_count: "        << this->m_fields.s_max_mnt_count << '\n';
    std::cout << "s_magic: "                << this->m_fields.s_magic << '\n';
    std::cout << "s_state: "                << this->m_fields.s_state << '\n';
    std::cout << "s_errors: "               << this->m_fields.s_errors << '\n';
    std::cout << "s_minor_rev_level: "      << this->m_fields.s_minor_rev_level << '\n';
    std::cout << "s_lastcheck: "            << this->m_fields.s_lastcheck << '\n';
    std::cout << "s_checkinterval: "        << this->m_fields.s_checkinterval << '\n';
    std::cout << "s_creator_os: "           << this->m_fields.s_creator_os << '\n';
    std::cout << "s_rev_level: "            << this->m_fields.s_rev_level << '\n';
    std::cout << "s_def_resuid: "           << this->m_fields.s_def_resuid << '\n';
    std::cout << "s_def_resgid: "           << this->m_fields.s_def_resgid << '\n';
}

