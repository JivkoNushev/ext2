#pragma once

#include "Block.h"

class SuperBlock : public Block
{
public:
    SuperBlock(); // calls load

private:
    uint32_t m_inodes_count = 0;
    uint32_t m_block_count = 0;
    uint32_t m_blocks_per_group = 0;
    uint32_t m_indoes_per_group = 0;
    uint32_t m_log_block_size = 0;

    bool m_fs_state = false;
    uint32_t m_first_data_block_number = 0;


    void load(); // loads parameters from disk
};
