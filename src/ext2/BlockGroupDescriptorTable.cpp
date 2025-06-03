#include <iostream>

#include "BlockGroupDescriptorTable.h"


BlockGroupDescriptorTable::BlockGroupDescriptorTable() :
    Block(0, 0)
{}

BlockGroupDescriptorTable::BlockGroupDescriptorTable(const SuperBlock& sb) :
    Block(sb.get_bg_count() * sizeof(BlockGroupDescriptor::Fields), BlockGroupDescriptorTable::OFFSET)
{
    this->m_gd_count = sb.get_bg_count();
    this->m_table = new BlockGroupDescriptor[this->m_gd_count];

    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        this->m_table[i] = BlockGroupDescriptor(sizeof(BlockGroupDescriptor::Fields), i * sizeof(BlockGroupDescriptor::Fields) + this->m_offset);
    }
}

BlockGroupDescriptorTable::~BlockGroupDescriptorTable()
{
    delete[] m_table;
    m_table = nullptr;
}

BlockGroupDescriptorTable::BlockGroupDescriptorTable(const BlockGroupDescriptorTable& table) :
    Block(table)
{
    this->copy_from(table);
}

BlockGroupDescriptorTable::BlockGroupDescriptorTable(BlockGroupDescriptorTable&& table) noexcept :
    Block(table)
{
    this->move_from(std::move(table));
}

BlockGroupDescriptorTable& BlockGroupDescriptorTable::operator=(const BlockGroupDescriptorTable& table)
{
    if(this != &table)
    {
        this->free();
        this->copy_from(table);
    }

    return *this;
}

BlockGroupDescriptorTable& BlockGroupDescriptorTable::operator=(BlockGroupDescriptorTable&& table) noexcept
{
    if(this != &table)
    {
        this->free();
        this->move_from(std::move(table));
    }

    return *this;
}

void BlockGroupDescriptorTable::print_fields() const
{
    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        std::cout << "Group " << i << ":\n";
        this->m_table[i].print_fields();
    }
}

uint32_t BlockGroupDescriptorTable::read(const char* file)
{
    uint32_t total_size = 0;
    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        total_size += this->m_table[i].read(file);
    }

    return total_size;
}

uint32_t BlockGroupDescriptorTable::write(const char* file) const
{
    uint32_t total_size = 0;
    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        total_size += this->m_table[i].write(file);
    }

    return total_size;
}

uint32_t BlockGroupDescriptorTable::write_bgd(const char* file, uint16_t index) const
{
    return this->m_table[index].write(file);
}

BlockGroupDescriptor& BlockGroupDescriptorTable::get_bgd(uint32_t group_num)
{
    return this->m_table[group_num];
}

const BlockGroupDescriptor& BlockGroupDescriptorTable::get_bgd(uint32_t group_num) const
{
    return this->m_table[group_num];
}

void BlockGroupDescriptorTable::free()
{
    delete[] m_table;
    this->m_table = nullptr;
}

void BlockGroupDescriptorTable::copy_from(const BlockGroupDescriptorTable& table)
{
    this->m_gd_count = table.m_gd_count;
    this->m_table = new BlockGroupDescriptor[this->m_gd_count];

    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        this->m_table[i] = table.m_table[i];
    }
}


void BlockGroupDescriptorTable::move_from(BlockGroupDescriptorTable&& table)
{
    this->m_table = table.m_table;
    this->m_gd_count = table.m_gd_count;

    table.m_table = nullptr;
    table.m_gd_count = 0;
}
