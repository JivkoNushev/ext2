#pragma once

#include <vector>
#include <string>

#include "../FileSystem.h"
#include "DirectoryEntry.h"
#include "SuperBlock.h"
#include "BlockGroupDescriptorTable.h"
#include "InodeTable.h"

class Ext2 : public FileSystem
{
public:
    Ext2(const char* device_path, bool format);
    ~Ext2() = default;

private:
    void load_ext2();
    void format_ext2() const;

    void read_block(uint32_t block_number, uint8_t* buffer);
    std::vector<LinkedDirectoryEntry> read_dir_entries(const Inode& inode);
    void read_directory_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size);
    void read_indirect_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size);
    void read_double_indirect_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size);
    void read_triple_indirect_block(uint32_t block_num, std::vector<LinkedDirectoryEntry>& entries, uint32_t block_size);

    void print_tree(uint32_t inode_idx, const std::string& prefix = "", bool is_last = true);

    SuperBlock m_sb;
    BlockGroupDescriptorTable m_bgdt;
    InodeTable m_it;
};
