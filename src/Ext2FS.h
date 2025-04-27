#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "SuperBlock.h"
#include "GroupDescriptor.h"
#include "Inode.h"


class Ext2FS {
public:
    Ext2FS(const std::string& filename);
    void mkdir(const std::string& path);
    void create_file(const std::string& path);
    void write_file(const std::string& path, const std::vector<uint8_t>& data);
    std::vector<uint8_t> read_file(const std::string& path);
    void delete_file(const std::string& path);
    void list_directory(const std::string& path);

private:
    void load();
    void format();
    void save();

    void allocate_inode();
    void allocate_block();

    std::fstream disk;
    SuperBlock superblock;
    std::vector<GroupDescriptor> group_descriptors;
    std::vector<uint8_t> inode_bitmap;
    std::vector<uint8_t> block_bitmap;
    std::vector<Inode> inode_table;
};
