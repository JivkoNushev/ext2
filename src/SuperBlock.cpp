#include "SuperBlock.h"

#include <exception>
#include <ctime>


SuperBlock::SuperBlock()
{
    this->format();
}

void SuperBlock::load(Disk& disk)
{
    disk.seek(SUPERBLOCK_OFFSET, Disk::Begin);
    disk.read((uint8_t*)&this->fields, sizeof(SuperBlockFields));

    if(this->fields.s_magic != SUPERBLOCK_MAGIC_BYTES)
    {
        throw std::exception();
    }
}

void SuperBlock::save(Disk& disk) const
{
    disk.seek(SUPERBLOCK_OFFSET, Disk::Begin);
    disk.write((uint8_t*)&this->fields, sizeof(SuperBlockFields));
}

void SuperBlock::format(uint32_t total_blocks, uint32_t total_inodes, uint32_t block_size)
{
    this->fields.s_inodes_count = total_inodes;
    this->fields.s_blocks_count = total_blocks;
    this->fields.s_r_blocks_count = 0; // Reserved blocks if you want
    this->fields.s_free_blocks_count = total_blocks - 1; // after superblock
    this->fields.s_free_inodes_count = total_inodes - 1; // after root inode
    this->fields.s_first_data_block = (block_size == 1024) ? 1 : 0;
    this->fields.s_log_block_size = (block_size == 1024) ? 0 : (block_size / 1024);
    this->fields.s_blocks_per_group = total_blocks; // for simplicity
    this->fields.s_inodes_per_group = total_inodes; // for simplicity
    this->fields.s_mtime = std::time(nullptr);
    this->fields.s_wtime = std::time(nullptr);
    this->fields.s_mnt_count = 0;
    this->fields.s_max_mnt_count = 20;
    this->fields.s_magic = this->SUPERBLOCK_MAGIC_BYTES;
    this->fields.s_state = 1; // cleanly mounted
    this->fields.s_errors = 1; // continue on errors
    this->fields.s_minor_rev_level = 0;
    this->fields.s_lastcheck = std::time(nullptr);
    this->fields.s_checkinterval = 0; // no forced fsck
    this->fields.s_creator_os = 0; // Linux
    this->fields.s_rev_level = 0; // original version
    this->fields.s_def_resuid = 0;
    this->fields.s_def_resgid = 0;
}
