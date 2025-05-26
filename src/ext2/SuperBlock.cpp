#include <cstdint>
#include <fstream>
#include <iostream>

#include "SuperBlock.h"

SuperBlock::SuperBlock() :
    Block(SB_SIZE, SB_OFFSET)
{}

SuperBlock::SuperBlock(uint32_t size, uint32_t offset) :
    Block(size, offset)
{}

SuperBlock::SuperBlock(uint32_t size, uint32_t offset, const char* device_path) :
    Block(size, offset)
{
    this->read(device_path);

    this->m_block_group_count = this->m_fields.s_blocks_count / this->m_fields.s_blocks_per_group + (this->m_fields.s_blocks_count % this->m_fields.s_blocks_per_group != 0);
    this->m_block_size = 1024 << this->m_fields.s_log_block_size;
}

uint32_t SuperBlock::read(const char* file_name)
{
    std::ifstream ifs(file_name, std::ios::binary);

    if (!ifs.is_open())
    {
        std::cerr << "[Error] Failed to open device " << file_name << " for reading SuperBlock.\n";
        throw std::runtime_error("Failed to open device for SuperBlock read");
    }

    ifs.seekg(this->m_offset, std::ios_base::beg);
    if (ifs.fail())
    {
        std::cerr << "[Error] Failed to seek to offset " << this->m_offset << " for reading SuperBlock in file " << file_name << '\n';
        ifs.close();
        throw std::runtime_error("Failed to seek in SuperBlock read");
    }

    ifs.read((char*)(&this->m_fields), sizeof(this->m_fields));
    if (ifs.fail())
    {
        std::cerr << "[Error] Failed to read SuperBlock data from file " << file_name << '\n';
        ifs.close();
        throw std::runtime_error("Failed to read SuperBlock data");
    }

    ifs.close();
    return sizeof(this->m_fields);
}

uint32_t SuperBlock::write(const char* file_name) const
{
    std::fstream fs(file_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!fs.is_open())
    {
        std::cerr << "[Error] Failed to open device " << file_name << " for writing SuperBlock.\n";
        throw std::runtime_error("Failed to open device for SuperBlock write");
    }

    fs.seekp(this->m_offset, std::ios_base::beg);
    if (fs.fail())
    {
        std::cerr << "[Error] Failed to seek to offset " << this->m_offset << " for reading SuperBlock in file " << file_name << '\n';
        fs.close();
        throw std::runtime_error("Failed to seek in SuperBlock read");
    }

    fs.write((const char*)(&this->m_fields), sizeof(this->m_fields));
    if (fs.fail())
    {
        std::cerr << "Error: Failed to read SuperBlock data from file " << file_name << '\n';
        fs.close();
        throw std::runtime_error("Failed to read SuperBlock data");
    }

    fs.flush();
    fs.close();
    return sizeof(this->m_fields);
}

uint16_t SuperBlock::get_bg_count() const
{
    return this->m_block_group_count;
}

uint32_t SuperBlock::get_block_size() const
{
    return this->m_block_size;
}

void SuperBlock::print_fields() const 
{
    std::cout << "s_inodes_count: "                     << this->m_fields.s_inodes_count << '\n';
    std::cout << "s_blocks_count: "                     << this->m_fields.s_blocks_count << '\n';
    std::cout << "s_r_blocks_count: "                   << this->m_fields.s_r_blocks_count << '\n';
    std::cout << "s_free_blocks_count: "                << this->m_fields.s_free_blocks_count << '\n';
    std::cout << "s_free_inodes_count: "                << this->m_fields.s_free_inodes_count << '\n';
    std::cout << "s_first_data_block: "                 << this->m_fields.s_first_data_block << '\n';
    std::cout << "s_log_block_size: "                   << this->m_fields.s_log_block_size << '\n';
    std::cout << "s_log_frag_size: "                    << this->m_fields.s_log_frag_size << '\n';
    std::cout << "s_blocks_per_group: "                 << this->m_fields.s_blocks_per_group << '\n';
    std::cout << "s_frags_per_group: "                  << this->m_fields.s_frags_per_group << '\n';
    std::cout << "s_inodes_per_group: "                 << this->m_fields.s_inodes_per_group << '\n';
    std::cout << "s_mtime: "                            << this->m_fields.s_mtime << '\n';
    std::cout << "s_wtime: "                            << this->m_fields.s_wtime << '\n';
    std::cout << "s_mnt_count: "                        << this->m_fields.s_mnt_count << '\n';
    std::cout << "s_max_mnt_count: "                    << this->m_fields.s_max_mnt_count << '\n';
    std::cout << "s_magic: "                            << this->m_fields.s_magic << '\n';
    std::cout << "s_state: "                            << this->m_fields.s_state << '\n';
    std::cout << "s_errors: "                           << this->m_fields.s_errors << '\n';
    std::cout << "s_minor_rev_level: "                  << this->m_fields.s_minor_rev_level << '\n';
    std::cout << "s_lastcheck: "                        << this->m_fields.s_lastcheck << '\n';
    std::cout << "s_checkinterval: "                    << this->m_fields.s_checkinterval << '\n';
    std::cout << "s_creator_os: "                       << this->m_fields.s_creator_os << '\n';
    std::cout << "s_rev_level: "                        << this->m_fields.s_rev_level << '\n';
    std::cout << "s_def_resuid: "                       << this->m_fields.s_def_resuid << '\n';
    std::cout << "s_def_resgid: "                       << this->m_fields.s_def_resgid << '\n';
    // EXT2_DYNAMIC_REV Specific
    std::cout << "s_first_ino: "                        << this->m_fields.s_first_ino << '\n';
    std::cout << "s_inode_size: "                       << this->m_fields.s_inode_size << '\n';
    std::cout << "s_block_group_nr: "                   << this->m_fields.s_block_group_nr << '\n';
    std::cout << "s_feature_compat: "                   << this->m_fields.s_feature_compat << '\n';
    std::cout << "s_feature_incompat: "                 << this->m_fields.s_feature_incompat << '\n';
    std::cout << "s_feature_ro_compat: "                << this->m_fields.s_feature_ro_compat << '\n';
    std::cout << "s_uuid: "                             << this->m_fields.s_uuid << '\n';
    std::cout << "s_volume_name: "                      << this->m_fields.s_volume_name << '\n';
    std::cout << "s_last_mounted: "                     << this->m_fields.s_last_mounted << '\n';
    std::cout << "s_algo_bitmap: "                      << this->m_fields.s_algo_bitmap << '\n';
    // Performance Hints
    std::cout << "s_prealloc_blocks: "                  << this->m_fields.s_prealloc_blocks << '\n';
    std::cout << "s_prealloc_dir_blocks: "              << this->m_fields.s_prealloc_dir_blocks << '\n';
    std::cout << "_s_perf_hints_allignment: "           << this->m_fields._s_perf_hints_allignment << '\n';
    // Journaling Support
    std::cout << "s_journal_uuid: "                     << this->m_fields.s_journal_uuid << '\n';
    std::cout << "s_journal_inum: "                     << this->m_fields.s_journal_inum << '\n';
    std::cout << "s_journal_dev: "                      << this->m_fields.s_journal_dev << '\n';
    std::cout << "s_last_orphan: "                      << this->m_fields.s_last_orphan << '\n';
    // Directory Indexing Support
    std::cout << "s_hash_seed: "                        << this->m_fields.s_hash_seed << '\n';
    std::cout << "s_def_hash_version: "                 << this->m_fields.s_def_hash_version << '\n';
    std::cout << "_s_dir_ind_sup_allignment: "          << this->m_fields._s_dir_ind_sup_allignment << '\n';
    // Other options
    std::cout << "s_default_mount_options: "            << this->m_fields.s_default_mount_options << '\n';
    std::cout << "s_first_meta_bg: "                    << this->m_fields.s_first_meta_bg << '\n';
    std::cout << "_s_reserved_for_future_rev: "         << this->m_fields._s_reserved_for_future_rev << '\n';
}
