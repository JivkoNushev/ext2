#pragma once

#include "SuperBlock.h"
#include "BlockGroupDescriptorTable.h"
#include "Inode.h"

class InodeTable : public Block
{
public:
// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    InodeTable();
    InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, uint16_t bg);
    ~InodeTable();

    InodeTable(const InodeTable& table);
    InodeTable(InodeTable&& table) noexcept;

    InodeTable& operator=(const InodeTable& table);
    InodeTable& operator=(InodeTable&& table) noexcept;

// ---------------- PUBLIC METHODS ----------------
    void print_fields(uint16_t inode_count = 2) const;

    uint32_t read(const char* file) override;

    uint32_t write(const char* file) const override;

    Inode& get_inode(uint32_t inode_number);

    const Inode& get_inode(uint32_t inode_number) const;

protected:
// ---------------- PROTECTED METHODS ----------------
    void* get_fields_buffer_for_read() override { return nullptr; }
    const void* get_fields_buffer_for_write() const override { return nullptr; };

private:
// ---------------- PRIVATE VARIABLES ----------------
    Inode* m_table = nullptr;
    uint16_t m_i_count = 0;

// ---------------- PRIVATE METHODS ----------------
    void free();
    void copy_from(const InodeTable& table);
    void move_from(InodeTable&& table);
};
