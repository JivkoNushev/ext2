#pragma once

#include "Block.h"

class Inode : public Block
{
public:
    Inode();
    Inode(uint32_t size, uint32_t offset);
    ~Inode() = default;

    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    void print_fields() const;

public:
    static constexpr const uint16_t I_SIZE = 128;

private:
    struct InodeFields
    {
        uint16_t i_mode = 0;
        uint16_t i_uid = 0;
        uint32_t i_size = 0;
        uint32_t i_atime = 0;
        uint32_t i_ctime = 0;
        uint32_t i_mtime = 0;
        uint32_t i_dtime = 0;
        uint16_t i_gid = 0;
        uint16_t i_links_count = 0;
        uint32_t i_blocks = 0;
        uint32_t i_flags = 0;
        uint32_t i_osd1 = 0;
        uint32_t i_block[15]{};
        uint32_t i_generation = 0;
        uint32_t i_file_acl = 0;
        uint32_t i_dir_acl = 0;
        uint32_t i_faddr = 0;
        uint8_t  i_osd2[12]{};
    };

    InodeFields m_fields;
};
