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
    void tree(const char* path) const override;
    void cat(const char* path) const override;

private:
// ---------------- PRIVATE VARIABLES ----------------
    SuperBlock m_sb;
    BlockGroupDescriptorTable m_bgdt;
    InodeTable m_it;

// ---------------- PRIVATE METHODS ----------------
    void load_ext2();
    void format_ext2() const;

    static uint32_t get_entry_with_name(const utils::vector<LinkedDirectoryEntry> entries, utils::string component_name);

    static uint16_t calculate_rec_len(uint8_t name_length);

    Inode& resolve_path(const utils::string& path, uint32_t& out_inode_num);

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

    uint32_t allocate_inode(uint32_t parent_inode, uint8_t*& out_inode_bitmap, uint32_t& out_inode_bitmap_num);

    uint32_t allocate(uint32_t preferred_group, uint16_t items_per_group, uint8_t*& out_bitmap, uint32_t& out_bitmap_num);

    uint32_t allocate_block(uint32_t inode_num, uint8_t*& out_block_bitmap_data, uint32_t& out_block_bitmap_num);

    void init_directory_block(Inode& new_dir_inode, uint32_t self_inode_num, uint32_t parent_inode_num);

    bool try_split_active_entry(LinkedDirectoryEntry* entry, uint8_t* buffer, uint32_t offset, uint32_t inode_num, const utils::string& name, uint8_t file_type, uint16_t entry_len);

    bool try_reuse_hole(LinkedDirectoryEntry* hole, uint32_t inode_num, const utils::string& name, uint8_t file_type, uint16_t entry_len);

    void fill_entry(LinkedDirectoryEntry* entry, uint32_t inode_num, const utils::string& name, uint8_t file_type);

    bool write_entry(
        uint32_t parent_block_num,
        uint32_t new_entry_inode_num, 
        const utils::string& new_entry_name, 
        uint8_t new_entry_file_type,
        uint16_t entry_len
    ) ;

    void append_dir_entry(
        Inode& parent_inode,
        uint32_t entry_num,
        const utils::string& entry_name,
        uint8_t entry_file_type
    );

    void write_inode(const Inode& inode, uint32_t inode_num);

    void write_bgd(const BlockGroupDescriptor& group_descriptor_to_write, uint32_t group_num);

    void write_sb(const SuperBlock& super_block_to_write);

    void commit_file(
        bool is_directory, uint32_t new_block_num,
        const Inode& new_inode, uint32_t new_inode_num,
        const Inode& parent_inode, uint32_t parent_inode_num,
        uint8_t* inode_bitmap, uint32_t inode_bitmap_num,
        uint8_t* block_bitmap, uint32_t block_bitmap_num
    );

    void create_file(const utils::string& path, bool is_directory);

    void deallocate_inode_on_disk(uint32_t inode_num, bool is_directory);

    void deallocate_block_on_disk(uint32_t block_num);

    void commit_deallocated_file(Inode& entry_inode, uint32_t entry_inode_num, Inode& parent_inode, uint32_t parent_inode_num);

    bool find_and_remove_entry_from_data_block(Inode& parent_inode, uint32_t entry_data_block_num, const utils::string& entry_name);

    uint32_t get_entry_data_block_and_number(const Inode& parent_inode, const utils::string& entry_name, uint32_t& entry_inode_number);

    bool has_entry_children(const Inode& entry_inode);

    void remove_entry_children(const utils::string& path, const Inode& entry_inode);

    bool remove_file(const utils::string& path, bool recursive);

    void process_block(utils::vector<uint8_t>& file_data, uint32_t file_size, uint32_t& bytes_read, uint32_t block_num, uint8_t* block_buffer);

    void process_single_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer);

    void process_double_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer);

    void process_triple_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer);

    utils::vector<uint8_t> read_file_data(const Inode& inode);

    utils::vector<uint8_t> read_file(const utils::string& path);
};
