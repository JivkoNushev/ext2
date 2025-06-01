#include <iostream>

#include "Inode.h"

Inode::Inode() :
    Block(sizeof(Inode::Fields), 0)
{}

Inode::Inode(uint32_t size, uint32_t offset) :
    Block(size, offset)
{}

Inode::Inode(bool is_directory, uint32_t block_size, uint32_t new_block_num) :
    Block(sizeof(Inode::Fields), 0)
{
    this->init(is_directory, block_size, new_block_num);
}

void Inode::print_fields() const
{
    std::cout << "i_mode: "                         << this->m_fields.i_mode << '\n';
    std::cout << "i_uid: "                          << this->m_fields.i_uid << '\n';
    std::cout << "i_size: "                         << this->m_fields.i_size << '\n';
    std::cout << "i_atime: "                        << this->m_fields.i_atime << '\n';
    std::cout << "i_ctime: "                        << this->m_fields.i_ctime << '\n';
    std::cout << "i_mtime: "                        << this->m_fields.i_mtime << '\n';
    std::cout << "i_dtime: "                        << this->m_fields.i_dtime << '\n';
    std::cout << "i_gid: "                          << this->m_fields.i_gid << '\n';
    std::cout << "i_links_count: "                  << this->m_fields.i_links_count << '\n';
    std::cout << "i_blocks: "                       << this->m_fields.i_blocks << '\n';
    std::cout << "i_flags: "                        << this->m_fields.i_flags << '\n';
    std::cout << "i_osd1: "                         << this->m_fields.i_osd1 << '\n';
    std::cout << "i_block: "                        << this->m_fields.i_block << '\n';
    std::cout << "i_generation: "                   << this->m_fields.i_generation << '\n';
    std::cout << "i_file_acl: "                     << this->m_fields.i_file_acl << '\n';
    std::cout << "i_dir_acl: "                      << this->m_fields.i_dir_acl << '\n';
    std::cout << "i_faddr: "                        << this->m_fields.i_faddr << '\n';
    std::cout << "i_osd2: "                         << this->m_fields.i_osd2 << '\n';
}

void Inode::init(bool is_directory, uint32_t block_size, uint32_t new_block_num)
{
    uint16_t mode = is_directory ? (Inode::Mode::EXT2_S_IFDIR | 0755) : (Inode::Mode::EXT2_S_IFREG | 0644);
    this->m_fields.i_mode = mode;
    this->m_fields.i_size = is_directory ? block_size : 0;
    this->m_fields.i_block[0] = new_block_num;
    this->m_fields.i_blocks = (block_size + 511) / 512;
    this->m_fields.i_links_count = is_directory ? 2 : 1;
    this->set_times_now();
}

void Inode::set_times_now()
{
    this->m_fields.i_atime = time(nullptr);
    this->m_fields.i_ctime = time(nullptr);
    this->m_fields.i_mtime = time(nullptr);
    this->m_fields.i_dtime = 0;
}

void* Inode::get_fields_buffer_for_read()
{
    return &this->m_fields;
}

const void* Inode::get_fields_buffer_for_write() const
{
    return &this->m_fields;
}
