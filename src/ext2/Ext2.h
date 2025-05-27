#pragma once

#include "../FileSystem.h"
#include "../utils/utils.h"
#include "DirectoryEntry.h"
#include "SuperBlock.h"
#include "BlockGroupDescriptorTable.h"
#include "InodeTable.h"

class Ext2 : public FileSystem
{
public:
// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    Ext2(const char* device_path, bool format);
    ~Ext2() = default;

// ---------------- PUBLIC METHODS ----------------
    void tree(const char* path) override;

private:
// ---------------- PRIVATE VARIABLES ----------------
    SuperBlock m_sb;
    BlockGroupDescriptorTable m_bgdt;
    InodeTable m_it;

// ---------------- PRIVATE METHODS ----------------
    void load_ext2();
    void format_ext2() const;

    Inode resolve_path(const utils::string& path, uint32_t& out_inode_num);

    utils::vector<LinkedDirectoryEntry> read_dir_entries(const Inode& inode);

    void read_directory_block(uint32_t block_num, utils::vector<LinkedDirectoryEntry>& entries, uint32_t block_size);

    utils::vector<uint32_t> read_indirect_block_pointers(uint32_t indirect_block_num, uint32_t block_size);

    void read_single_indirect_block(uint32_t indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size);

    void read_double_indirect_block(uint32_t double_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size);

    void read_triple_indirect_block(uint32_t triple_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size);

    void print_tree(uint32_t inode_idx, const utils::string& prefix = utils::string(), bool is_last = true);
};
