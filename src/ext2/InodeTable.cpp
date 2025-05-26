#include <iostream>

#include "InodeTable.h"

InodeTable::InodeTable() :
    Block(0, 0)
{}

InodeTable::InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, uint16_t bg) :
    Block(sb.m_fields.s_inodes_per_group * sizeof(Inode::Fields), sb.get_block_size() * bgdt.get_bgd(bg).m_fields.bg_inode_table)
{
    this->m_i_count = sb.m_fields.s_inodes_per_group;
    this->m_table = new Inode[this->m_i_count];

    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        this->m_table[i] = Inode(sizeof(Inode::Fields), i * sb.m_fields.s_inode_size+ this->m_offset);
    }
}

InodeTable::~InodeTable()
{
    delete[] m_table;
    m_table = nullptr;
}

InodeTable::InodeTable(const InodeTable& table) :
    Block(table)
{
    this->copy_from(table);
}

InodeTable::InodeTable(InodeTable&& table) noexcept :
    Block(table)
{
    this->move_from(std::move(table));
}

InodeTable& InodeTable::operator=(const InodeTable& table)
{
    if(this != &table)
    {
        this->free();
        this->copy_from(table);
    }

    return *this;
}

InodeTable& InodeTable::operator=(InodeTable&& table) noexcept
{
    if(this != &table)
    {
        this->free();
        this->move_from(std::move(table));
    }

    return *this;
}

void InodeTable::print_fields(uint16_t inode_count) const
{
    for(uint32_t i = 0; i < inode_count; i++)
    {
        std::cout << "Inode " << i << ":\n";
        this->m_table[i].print_fields();
    }
}

uint32_t InodeTable::read(const char* file)
{
    uint32_t total_size = 0;
    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        total_size += this->m_table[i].read(file);
    }

    return total_size;
}

uint32_t InodeTable::write(const char* file) const
{
    uint32_t total_size = 0;
    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        total_size += this->m_table[i].write(file);
    }

    return total_size;
}


Inode& InodeTable::get_inode(uint32_t inode_number)
{
    uint32_t index = inode_number - 1;
    if(this->m_i_count <= index) throw std::out_of_range("[Error] Invalid inode number");

    return this->m_table[index];
}

const Inode& InodeTable::get_inode(uint32_t inode_number) const
{
    uint32_t index = inode_number - 1;
    if(this->m_i_count <= index) throw std::out_of_range("[Error] Invalid inode number");

    return this->m_table[index];
}

void InodeTable::free()
{
    delete[] m_table;
    this->m_table = nullptr;
}

void InodeTable::copy_from(const InodeTable& table)
{
    this->m_i_count = table.m_i_count;
    this->m_table = new Inode[this->m_i_count];

    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        this->m_table[i] = table.m_table[i];
    }
}


void InodeTable::move_from(InodeTable&& table)
{
    this->m_table = table.m_table;
    this->m_i_count = table.m_i_count;

    table.m_table = nullptr;
    table.m_i_count = 0;
}
