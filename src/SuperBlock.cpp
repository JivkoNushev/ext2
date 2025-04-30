#include "SuperBlock.h"

#include <exception>
#include <ctime>
#include <stdexcept>


SuperBlock::SuperBlock(uint32_t offset) : fields()
{
    this->offset = offset;
}

void SuperBlock::load(Disk& disk)
{
    disk.seek(this->offset, Disk::Begin);
    disk.read((uint8_t*)&this->fields, this->size);

    if(this->fields.s_magic != SuperBlock::MAGIC_BYTES)
    {
        throw std::invalid_argument("Magic bytes don't match\n");
    }
}

void SuperBlock::save(Disk& disk) const
{
    disk.seek(this->offset, Disk::Begin);
    disk.write((uint8_t*)&this->fields, this->size);
}

void SuperBlock::format(uint32_t blocks_per_group, uint32_t inodes_per_group, uint32_t disk_size)
{
    const uint32_t total_blocks = disk_size / this->size;
    const uint32_t total_inodes = (total_blocks / blocks_per_group) * inodes_per_group;

    this->fields.s_inodes_count = total_inodes;
    this->fields.s_blocks_count = total_blocks;
    this->fields.s_r_blocks_count = 0;
    this->fields.s_free_blocks_count = total_blocks;
    this->fields.s_free_inodes_count = total_inodes;
    this->fields.s_first_data_block = 1;
    this->fields.s_log_block_size = 0;
    this->fields.s_log_frag_size = 0;
    this->fields.s_blocks_per_group = blocks_per_group;
    this->fields.s_frags_per_group = blocks_per_group;
    this->fields.s_inodes_per_group = inodes_per_group;
    this->fields.s_magic = SuperBlock::MAGIC_BYTES;
    this->fields.s_state = 1;
    this->fields.s_errors = 1;
    this->fields.s_minor_rev_level = 0;
    this->fields.s_lastcheck = 0;
    this->fields.s_checkinterval = 0;
    this->fields.s_creator_os = 0;
    this->fields.s_rev_level = 0;
    this->fields.s_def_resuid = 0;
    this->fields.s_def_resgid = 0;
}
