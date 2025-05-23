#pragma once

#include "Block.h"

class Inode : public Block
{
public:
    enum Mode : uint16_t
    {
        // -- file format --
        EXT2_S_IFSOCK = 0xC000, // socket
        EXT2_S_IFLNK  = 0xA000, // symbolic link
        EXT2_S_IFREG  = 0x8000, // regular file
        EXT2_S_IFBLK  = 0x6000, // block device
        EXT2_S_IFDIR  = 0x4000, // directory
        EXT2_S_IFCHR  = 0x2000, // character device
        EXT2_S_IFIFO  = 0x1000, // fifo
        // -- process execution user/group override --
        EXT2_S_ISUID  = 0x0800, // Set process User ID
        EXT2_S_ISGID  = 0x0400, // Set process Group ID
        EXT2_S_ISVTX  = 0x0200, // sticky bit
        // -- access rights --
        EXT2_S_IRUSR  = 0x0100, // user read
        EXT2_S_IWUSR  = 0x0080, // user write
        EXT2_S_IXUSR  = 0x0040, // user execute
        EXT2_S_IRGRP  = 0x0020, // group read
        EXT2_S_IWGRP  = 0x0010, // group write
        EXT2_S_IXGRP  = 0x0008, // group execute
        EXT2_S_IROTH  = 0x0004, // others read
        EXT2_S_IWOTH  = 0x0002, // others write
        EXT2_S_IXOTH  = 0x0001  // others execute
    };
public:
    Inode();
    Inode(uint32_t size, uint32_t offset);
    ~Inode() = default;

    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    void print_fields() const;

    uint32_t get_block(uint32_t index) const;
    uint16_t get_mode() const;

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
