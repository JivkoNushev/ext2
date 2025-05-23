#include <iostream>
#include <stdexcept>

#include "InodeTable.h"
#include "Inode.h"


InodeTable::InodeTable() :
    Block(0, 0)
{}

InodeTable::InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, uint16_t bg) :
    Block(sb.get_inodes_count() * sb.get_inode_size(), sb.get_block_size() * bgdt.get_inode_table(bg))
{
    uint16_t inode_size = sb.get_inode_size();
    this->m_i_count = sb.get_inodes_count();
    this->m_table = new Inode[this->m_i_count];

    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        this->m_table[i] = Inode(inode_size, i * inode_size + this->m_offset);
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

uint32_t InodeTable::read(const char* file)
{
    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        Inode& inode = this->m_table[i];
        inode.read(file);
    }

    return this->m_size;
}

uint32_t InodeTable::write(const char* file) const
{
    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        this->m_table[i].write(file);
    }

    return this->m_size;
}

void InodeTable::print_fields() const
{
    uint32_t count_to = this->m_i_count;
    for(uint32_t i = 0; i < count_to; i++)
    {
        std::cout << "Inode " << i << ":\n";
        this->m_table[i].print_fields();
    }
}

Inode& InodeTable::get_inode(uint32_t inode_number)
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
