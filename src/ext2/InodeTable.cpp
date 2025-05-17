#include <iostream>

#include "InodeTable.h"
#include "Inode.h"


InodeTable::InodeTable() :
    Block(0, 0)
{}

InodeTable::InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, uint16_t bg) :
    Block(sb.get_inodes_count() * Inode::I_SIZE, sb.get_block_size() * bgdt.get_inode_table(bg))
{
    this->m_i_count = sb.get_bg_count();
    this->m_table = new Inode[this->m_i_count];

    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        this->m_table[i] = Inode(Inode::I_SIZE, i * Inode::I_SIZE + this->m_offset);
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
        Inode& bgd = this->m_table[i];
        bgd.read(file);
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
    for(uint16_t i = 0; i < this->m_i_count; i++)
    {
        std::cout << "Inode " << i << ":\n";
        this->m_table[i].print_fields();
    }
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
