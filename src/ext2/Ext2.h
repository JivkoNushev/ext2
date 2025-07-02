#pragma once

#include "../FileSystem.h"
#include "../utils/utils.h"
#include "DirectoryEntry.h"
#include "SuperBlock.h"
#include "BlockGroupDescriptorTable.h"
#include "InodeTable.h"

class FileWriteContext;

struct NewDirEntry
{
    uint32_t inode_num;
    const utils::string& name;
    uint8_t file_type;
    uint16_t required_len() const;
};

struct FileCreationCommitInfo
{
    bool is_directory;
    const Inode& new_inode;
    uint32_t new_inode_num;
    uint32_t new_block_num;
    const Inode& parent_inode;
    uint32_t parent_inode_num;
    uint8_t* inode_bitmap_data;
    uint32_t inode_bitmap_block_num;
    uint8_t* block_bitmap_data;
    uint32_t block_bitmap_block_num;
};

class Ext2 : public FileSystem
{
    friend class FileWriteContext;

public:
    Ext2(const char* device_path, bool format);
    ~Ext2() = default;
    void tree(const char* path) const noexcept override;
    void cat(const char* path) const noexcept override;
    void write(const char* path, const char* data, bool append) noexcept override;
    void touch(const char* path) noexcept override;
    void mkdir(const char* path) noexcept override;
    void rm(const char* path, bool recursive = false) noexcept override;

    static uint16_t calculate_rec_len(uint8_t name_length);

private:
    SuperBlock m_sb;
    BlockGroupDescriptorTable m_bgdt;
    InodeTable m_it;

    void load_ext2();
    void format_ext2() const;

    Inode& resolve_path(const utils::string& path, uint32_t& out_inode_num);
    static uint32_t get_entry_with_name(const utils::vector<LinkedDirectoryEntry> entries, utils::string component_name);

    utils::vector<LinkedDirectoryEntry> read_dir_entries(const Inode& inode) const;
    void read_directory_block(uint32_t block_num, utils::vector<LinkedDirectoryEntry>& entries) const;
    utils::vector<uint32_t> read_indirect_block_pointers(uint32_t indirect_block_num) const;
    void read_single_indirect_block(uint32_t indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries) const;
    void read_double_indirect_block(uint32_t double_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries) const;
    void read_triple_indirect_block(uint32_t triple_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries) const;
    void print_tree(uint32_t inode_idx, const utils::string& prefix = utils::string(), bool is_last = true) const;

    void read_block(uint32_t block_number, uint8_t* buffer, uint32_t block_size) const;
    void write_block(uint32_t block_number, uint8_t* buffer, uint32_t block_size) const;

    static void check_entry_name_validity(const utils::string& name);
    uint32_t allocate(uint32_t preferred_group, uint16_t items_per_group, uint8_t*& out_bitmap, uint32_t& out_bitmap_num);
    uint32_t allocate_inode(uint32_t parent_inode, uint8_t*& out_inode_bitmap, uint32_t& out_inode_bitmap_num);
    uint32_t allocate_block(uint32_t inode_num, uint8_t*& out_block_bitmap_data, uint32_t& out_block_bitmap_num);

    void create_file(const utils::string& path, bool is_directory);
    void init_directory_block(Inode& new_dir_inode, uint32_t self_inode_num, uint32_t parent_inode_num);
    void fill_entry(LinkedDirectoryEntry* entry, uint32_t inode_num, const utils::string& name, uint8_t file_type);
    bool try_reuse_hole(LinkedDirectoryEntry* hole, const NewDirEntry& new_entry);
    bool try_split_active_entry(LinkedDirectoryEntry* entry, uint8_t* buffer, uint32_t offset, const NewDirEntry& new_entry);
    bool write_entry(uint32_t parent_block_num, const NewDirEntry& new_entry);
    void append_dir_entry(Inode& parent_inode, uint32_t inode_num, const utils::string& entry_name, uint8_t entry_file_type);
    void commit_file(const FileCreationCommitInfo& info);

    void remove_file(const utils::string& path, bool recursive);
    bool has_entry_children(const Inode& entry_inode);
    void remove_entry_children(const utils::string& path, const Inode& entry_inode);
    bool find_and_remove_entry_from_data_block(Inode& parent_inode, uint32_t entry_data_block_num, const utils::string& entry_name);
    uint32_t get_entry_data_block_and_number(const Inode& parent_inode, const utils::string& entry_name, uint32_t& entry_inode_number);
    void commit_deallocated_file(Inode& entry_inode, uint32_t entry_inode_num, Inode& parent_inode, uint32_t parent_inode_num);

    void deallocate_inode_on_disk(uint32_t inode_num, bool is_directory);
    void deallocate_block_on_disk(uint32_t block_num);
    void deallocate_inode_content(Inode& inode);
    void deallocate_direct_blocks(Inode& inode);
    void deallocate_indirect_blocks_recursive(uint32_t block_num, int level);

    void write_inode(const Inode& inode, uint32_t inode_num);
    void write_bgd(const BlockGroupDescriptor& group_descriptor_to_write, uint32_t group_num);
    void write_sb(const SuperBlock& super_block_to_write);

    utils::vector<uint8_t> read_file(const utils::string& path);
    utils::vector<uint8_t> read_file_data(const Inode& inode);
    void process_block(utils::vector<uint8_t>& file_data, uint32_t file_size, uint32_t& bytes_read, uint32_t block_num, uint8_t* block_buffer);
    void process_single_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer);
    void process_double_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer);
    void process_triple_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer);

    void write_file(const utils::string& path, const utils::vector<uint8_t>& data_to_write, bool append);
    void perform_inode_data_write(Inode& inode, uint32_t inode_num, const utils::vector<uint8_t>& data_to_write);
    void process_indirect_block_writes(FileWriteContext& context, int level, uint32_t* block_num_ptr);
};
