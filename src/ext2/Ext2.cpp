#include <iostream>
#include <stdexcept>
#include <cstring>
#include <fstream>

#include "Ext2.h"
#include "BlockGroupDescriptorTable.h"
#include "DirectoryEntry.h"
#include "SuperBlock.h"


Ext2::Ext2(const char* device_path, bool format) :
    FileSystem(device_path, FSType::ext2, format)
{
    if(format)
    {
        this->format_ext2();
    }
    else
    {
        this->load_ext2();
    }

    this->m_sb.print_fields();
    this->m_bgdt.print_fields();
    // this->m_it.print_fields();

    this->print_tree(2);
}

void Ext2::print_tree(uint32_t inode_idx, const std::string& prefix, bool is_last)
{
    const Inode& node = this->m_it.get_inode(inode_idx);

    // root inode
    if (prefix.empty()) {
        std::cout << "/ (inode " << inode_idx << ")" << std::endl;
    }

    // check if a directory
    if (!(node.get_mode() & Inode::Mode::EXT2_S_IFDIR)) return;

    std::vector<LinkedDirectoryEntry> entries = this->read_dir_entries(node);

    // Skip '.' and '..'
    std::vector<LinkedDirectoryEntry> children;
    for (LinkedDirectoryEntry& e : entries)
    {
        if (std::strcmp((char*)e.name, ".") == 0) continue;
        if (std::strcmp((char*)e.name, "..") == 0) continue;
        children.push_back(e);
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        bool last = (i + 1 == children.size());

        const LinkedDirectoryEntry& e = children[i];
        std::cout << prefix;
        std::cout << (last ? "└── " : "├── ");
        std::cout << e.name;

        // Mark dirs
        Inode& child_inode = this->m_it.get_inode(e.inode);
        if (child_inode.get_mode() & Inode::Mode::EXT2_S_IFDIR) std::cout << "/";
        std::cout << " (inode " << e.inode << ")";
        std::cout << std::endl;

        // Recurse into directories
        if (child_inode.get_mode() & Inode::Mode::EXT2_S_IFDIR)
        {
            print_tree(e.inode,
                       prefix + (last ? "    " : "│   "),
                       last);
        }
    }
}

void Ext2::format_ext2() const
{
    throw std::invalid_argument("[Error] Formatting ext2 is not implemented");
}

void Ext2::load_ext2()
{
    std::cout << "Loading ext2 from: " << this->get_device_path() << '\n';

    this->m_sb = SuperBlock(SuperBlock::SB_SIZE, SuperBlock::SB_OFFSET),
    this->m_sb.read(this->get_device_path());
    this->m_sb.load();

    this->m_bgdt = BlockGroupDescriptorTable(this->m_sb, 0);
    this->m_bgdt.read(this->get_device_path());

    this->m_it = InodeTable(this->m_sb, this->m_bgdt, 0);
    this->m_it.read(this->get_device_path());
}

void Ext2::read_block(uint32_t block_number, uint8_t* buffer)
{
    uint32_t block_size = this->m_sb.get_block_size();

    std::ifstream ifs(this->get_device_path());
    ifs.seekg(block_number * block_size, std::ios_base::beg);
    ifs.read((char*)buffer, block_size);
}

std::vector<LinkedDirectoryEntry> Ext2::read_dir_entries(const Inode& inode)
{
    std::vector<LinkedDirectoryEntry> entries;
    uint32_t block_size = this->m_sb.get_block_size();

    // Read direct blocks
    for (int i = 0; i < 12; ++i)
    {
        uint32_t block_num = inode.get_block(i);
        if (block_num == 0) continue;
        this->read_directory_block(block_num, entries, block_size);
    }

    // Read single indirect block
    if (uint32_t indirect = inode.get_block(12); indirect != 0)
    {
        this->read_indirect_block(indirect, entries, block_size);
    }

    // Read double indirect block
    if (uint32_t double_indirect = inode.get_block(13); double_indirect != 0)
    {
        this->read_double_indirect_block(double_indirect, entries, block_size);
    }

    // Read triple indirect block
    if (uint32_t triple_indirect = inode.get_block(14); triple_indirect != 0)
    {
        this->read_triple_indirect_block(triple_indirect, entries, block_size);
    }

    return entries;
}

void Ext2::read_directory_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size)
{
    uint8_t* block = new uint8_t[block_size];
    this->read_block(block_num, block);

    uint32_t offset = 0;
    while (offset + sizeof(LinkedDirectoryEntry) <= block_size)
    {
        LinkedDirectoryEntry* de = (LinkedDirectoryEntry*)(block + offset);

        if (de->rec_len < 8 || offset + de->rec_len > block_size)
            break;

        if (de->inode == 0)
        {
            offset += de->rec_len;
            continue;
        }

        LinkedDirectoryEntry entry;
        entry.inode = de->inode;
        entry.rec_len = de->rec_len;
        entry.name_len = de->name_len;
        entry.file_type = de->file_type;

        uint8_t name_len = std::min(de->name_len, (uint8_t)(sizeof(entry.name) - 1));
        std::memcpy(entry.name, de->name, name_len);
        entry.name[name_len] = '\0';

        entries.push_back(entry);

        offset += de->rec_len;
    }

    delete[] block;
}

void Ext2::read_indirect_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size)
{
    uint8_t* buffer = new uint8_t[block_size];
    this->read_block(block_num, buffer);

    uint32_t pointers_per_block = block_size / sizeof(uint32_t);
    uint32_t* pointers = reinterpret_cast<uint32_t*>(buffer);

    for (uint32_t i = 0; i < pointers_per_block; ++i)
    {
        if (pointers[i] == 0) continue;
        read_directory_block(pointers[i], entries, block_size);
    }
}

void Ext2::read_double_indirect_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size)
{
    uint8_t* buffer = new uint8_t[block_size];
    this->read_block(block_num, buffer);

    uint32_t pointers_per_block = block_size / sizeof(uint32_t);
    uint32_t* pointers = reinterpret_cast<uint32_t*>(buffer);

    for (uint32_t i = 0; i < pointers_per_block; ++i)
    {
        if (pointers[i] == 0) continue;
        read_indirect_block(pointers[i], entries, block_size);
    }
}

void Ext2::read_triple_indirect_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size)
{
    uint8_t* buffer = new uint8_t[block_size];
    this->read_block(block_num, buffer);

    uint32_t pointers_per_block = block_size / sizeof(uint32_t);
    uint32_t* pointers = reinterpret_cast<uint32_t*>(buffer);

    for (uint32_t i = 0; i < pointers_per_block; ++i)
    {
        if (pointers[i] == 0) continue;
        read_double_indirect_block(pointers[i], entries, block_size);
    }
}
