#include <fstream>
#include <iostream>

#include "Inode.h"

Inode::Inode() :
    Block(0, 0)
{}

Inode::Inode(uint32_t size, uint32_t offset) :
    Block(size, offset)
{}

uint32_t Inode::read(const char* file_name) 
{
    std::ifstream ifs(file_name);
    ifs.seekg(this->m_offset, std::ios_base::beg);
    ifs.read((char*)&this->m_fields, sizeof(this->m_fields));

    return sizeof(this->m_fields);
}

uint32_t Inode::write(const char* file) const
{
    (void)file;

    //TODO: Implement
    return 0;
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

