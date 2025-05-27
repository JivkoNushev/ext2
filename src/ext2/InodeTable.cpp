#include <iostream>

#include "InodeTable.h"

InodeTable::InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, const char* const& device_path) :
    Block(0, 0),
    m_sb(sb),
    m_bgdt(bgdt),
    m_device_path(device_path)
{}

InodeTable::~InodeTable()
{
    this->free();
}

InodeTable::InodeTable(const InodeTable& table) :
    Block(table),
    m_sb(table.m_sb),
    m_bgdt(table.m_bgdt),
    m_device_path(table.m_device_path)
{
    this->copy_from(table);
}

InodeTable::InodeTable(InodeTable&& table) noexcept :
    Block(table),
    m_sb(table.m_sb),
    m_bgdt(table.m_bgdt),
    m_device_path(table.m_device_path)

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
    if (0 == inode_number || inode_number > this->m_sb.m_fields.s_inodes_count)
    {
         throw std::out_of_range("InodeTable::get_inode - Invalid inode number: " + std::to_string(inode_number)); 
    }

    uint16_t required_group_idx = (uint16_t)((inode_number - 1) / this->m_sb.m_fields.s_inodes_per_group);
    if (!this->m_currently_loaded_group_idx || *this->m_currently_loaded_group_idx != required_group_idx)
    {
        if (!load_group(required_group_idx))
        {
            throw std::runtime_error("InodeTable::get_inode - Failed to load required group: " + std::to_string(required_group_idx));
        }
    }

    uint16_t index_in_group = (uint16_t)((inode_number - 1) % this->m_sb.m_fields.s_inodes_per_group);
    if (index_in_group >= this->m_i_count || this->m_table == nullptr)
    { 
        throw std::out_of_range("InodeTable::get_inode - Calculated index is out of bounds: " + std::to_string(index_in_group));
    }

    return this->m_table[index_in_group];
}

bool InodeTable::load_group(uint16_t group_idx)
{
    if (m_currently_loaded_group_idx && *m_currently_loaded_group_idx == group_idx)
    {
        return true;
    }


    if (group_idx >= m_sb.get_bg_count())
    {
        return false;
    }

    free();

    const BlockGroupDescriptor& bgd_for_group = m_bgdt.get_bgd(group_idx);
    uint32_t group_inode_table_start_block = bgd_for_group.m_fields.bg_inode_table;

    this->m_offset = m_sb.get_block_size() * group_inode_table_start_block;

    uint16_t inodes_in_this_group = m_sb.m_fields.s_inodes_per_group;
    uint16_t actual_inode_disk_size = m_sb.m_fields.s_inode_size; 
    this->m_size = inodes_in_this_group * actual_inode_disk_size; 

    this->m_i_count = inodes_in_this_group;
    if (0 == this->m_i_count)
    {
        return true;
    }

    this->m_table = new Inode[this->m_i_count]; 
    for (uint16_t i = 0; i < this->m_i_count; i++)
    {
        uint32_t inode_absolute_disk_offset = this->m_offset + (i * actual_inode_disk_size);
        //FIXME: FIX SIZE PROBLEM
        this->m_table[i] = Inode(sizeof(Inode::Fields), inode_absolute_disk_offset);
    }

    this->read(m_device_path);

    m_currently_loaded_group_idx = group_idx;
    return true;
}

void InodeTable::free()
{
    delete[] m_table;
    this->m_table = nullptr;
    m_i_count = 0;
    m_currently_loaded_group_idx = std::nullopt;
    this->m_size = 0;
    this->m_offset = 0;
}

void InodeTable::copy_from(const InodeTable& other)
{
    if (!other.m_table || 0 == other.m_i_count)
    {
        this->m_size = 0;
        this->m_offset = 0;

        this->m_table = nullptr;
        this->m_i_count = 0;
        this->m_currently_loaded_group_idx = std::nullopt;
        return;
    }

    this->m_size = other.m_size;
    this->m_offset = other.m_offset;

    this->m_i_count = other.m_i_count;
    this->m_table = new Inode[this->m_i_count];

    for (uint16_t i = 0; i < this->m_i_count; i++)
    {
        this->m_table[i] = other.m_table[i];
    }
    this->m_currently_loaded_group_idx = other.m_currently_loaded_group_idx;
}


void InodeTable::move_from(InodeTable&& other)
{
    this->m_size = other.m_size;
    this->m_offset = other.m_offset;
    this->m_table = other.m_table;
    this->m_i_count = other.m_i_count;
    this->m_currently_loaded_group_idx = other.m_currently_loaded_group_idx;

    other.m_table = nullptr;
    other.m_i_count = 0;
    other.m_currently_loaded_group_idx = std::nullopt;
    other.m_size = 0;
    other.m_offset = 0;
}
