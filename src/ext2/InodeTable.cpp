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
         throw std::out_of_range("InodeTable::get_inode - Invalid inode number");
    }

    uint16_t group = (uint16_t)((inode_number - 1) / this->m_sb.m_fields.s_inodes_per_group);
    if (!this->m_loaded_group || *this->m_loaded_group != group)
    {
        if (!this->load_group(group))
        {
            throw std::runtime_error("InodeTable::get_inode - Failed to load required group");
        }
    }

    uint16_t inode_index = (uint16_t)((inode_number - 1) % this->m_sb.m_fields.s_inodes_per_group);
    if (inode_index >= this->m_i_count || this->m_table == nullptr)
    {
        throw std::out_of_range("InodeTable::get_inode - Calculated index is out of bounds");
    }

    return this->m_table[inode_index];
}

bool InodeTable::load_group(uint16_t group)
{
    if (this->m_loaded_group && *this->m_loaded_group == group)
    {
        return true;
    }

    if (group >= this->m_sb.get_bg_count()) return false;

    this->free();

    const BlockGroupDescriptor& bgd = this->m_bgdt.get_bgd(group);
    uint32_t inode_table = bgd.m_fields.bg_inode_table;

    this->m_offset = this->m_sb.get_block_size() * inode_table;

    uint16_t inode_size = this->m_sb.m_fields.s_inode_size;
    this->m_i_count = this->m_sb.m_fields.s_inodes_per_group;
    this->m_size = this->m_i_count * inode_size;

    this->m_table = new Inode[this->m_i_count];
    for (uint16_t i = 0; i < this->m_i_count; i++)
    {
        uint32_t inode_offset = this->m_offset + (i * inode_size);
        this->m_table[i] = Inode(inode_size, inode_offset);
    }

    this->read(this->m_device_path);

    this->m_loaded_group = group;
    return true;
}

void InodeTable::free()
{
    delete[] m_table;
    this->m_table = nullptr;
    this->m_i_count = 0;
    this->m_loaded_group = std::nullopt;
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
        this->m_loaded_group = std::nullopt;
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
    this->m_loaded_group = other.m_loaded_group;
}


void InodeTable::move_from(InodeTable&& other)
{
    this->m_size = other.m_size;
    this->m_offset = other.m_offset;
    this->m_table = other.m_table;
    this->m_i_count = other.m_i_count;
    this->m_loaded_group = other.m_loaded_group;

    other.m_table = nullptr;
    other.m_i_count = 0;
    other.m_loaded_group = std::nullopt;
    other.m_size = 0;
    other.m_offset = 0;
}
