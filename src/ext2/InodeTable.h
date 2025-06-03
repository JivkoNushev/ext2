#pragma once

#include <optional>

#include "SuperBlock.h"
#include "BlockGroupDescriptorTable.h"
#include "Inode.h"

class InodeTable : public Block
{
public:
// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    InodeTable(const SuperBlock& sb, const BlockGroupDescriptorTable& bgdt, const char* const& device_path);
    ~InodeTable() override;

    InodeTable(const InodeTable& table);
    InodeTable(InodeTable&& table) noexcept;

    InodeTable& operator=(const InodeTable& table);
    InodeTable& operator=(InodeTable&& table) noexcept;

// ---------------- PUBLIC METHODS ----------------
    void print_fields(uint16_t inode_count = 2) const;

    uint32_t read(const char* file) override;

    uint32_t write(const char* file) const override;

    Inode& get_inode(uint32_t inode_number);

protected:
// ---------------- PROTECTED METHODS ----------------
    void* get_fields_buffer_for_read() override { return nullptr; }
    const void* get_fields_buffer_for_write() const override { return nullptr; };

private:
// ---------------- PRIVATE VARIABLES ----------------
    Inode* m_table = nullptr;
    uint16_t m_i_count = 0;

    const SuperBlock& m_sb;
    const BlockGroupDescriptorTable& m_bgdt;
    const char* const& m_device_path;

    std::optional<uint16_t> m_loaded_group = std::nullopt;

// ---------------- PRIVATE METHODS ----------------
    bool load_group(uint16_t group);
    void free();
    void copy_from(const InodeTable& other);
    void move_from(InodeTable&& other);
};
