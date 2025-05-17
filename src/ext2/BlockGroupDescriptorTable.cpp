#include "BlockGroupDescriptorTable.h"
#include "BlockGroupDescriptor.h"

#include <iostream>
#include <stdexcept>

BlockGroupDescriptorTable::BlockGroupDescriptorTable() :
    Block(0, 0)
{}


BlockGroupDescriptorTable::BlockGroupDescriptorTable(const SuperBlock& sb, uint16_t bg) :
    Block(sb.get_bg_count() * BlockGroupDescriptor::GD_SIZE, BlockGroupDescriptorTable::BGDT_OFFSET)
{
    this->m_gd_count = sb.get_bg_count();
    this->m_table = new BlockGroupDescriptor[this->m_gd_count];

    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        this->m_table[i] = BlockGroupDescriptor(BlockGroupDescriptor::GD_SIZE, i * BlockGroupDescriptor::GD_SIZE + this->m_offset);
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

uint32_t BlockGroupDescriptorTable::read(const char* file)
{
    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        BlockGroupDescriptor& bgd = this->m_table[i];
        bgd.read(file);
    }

    return this->m_size;
}

uint32_t BlockGroupDescriptorTable::write(const char* file) const
{
    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        this->m_table[i].write(file);
    }

    return this->m_size;
}

void BlockGroupDescriptorTable::print_fields() const
{
    for(uint16_t i = 0; i < this->m_gd_count; i++)
    {
        std::cout << "Group " << i << ":\n";
        this->m_table[i].print_fields();
    }
}

uint16_t BlockGroupDescriptorTable::get_inode_table(uint16_t bg) const
{
    return this->m_table[bg].get_inode_table();
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
