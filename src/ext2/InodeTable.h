#pragma once

#include "Block.h"
#include "BlockGroupDescriptorTable.h"
#include "Inode.h"
#include "SuperBlock.h"

class InodeTable : public Block
{
public:
    InodeTable();
    InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, uint16_t bg);
    ~InodeTable();

    InodeTable(const InodeTable& table);
    InodeTable(InodeTable&& table) noexcept;

    InodeTable& operator=(const InodeTable& table);
    InodeTable& operator=(InodeTable&& table) noexcept;

    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    void print_fields() const;

    Inode& get_inode(uint32_t index);

public:
    static constexpr const uint16_t I_OFFSET = 2048;

private:
    void free();
    void copy_from(const InodeTable& table);
    void move_from(InodeTable&& table);

    Inode* m_table = nullptr;
    uint16_t m_i_count = 0;
};
