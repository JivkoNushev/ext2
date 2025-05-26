#pragma once

#include "SuperBlock.h"
#include "BlockGroupDescriptor.h"

class BlockGroupDescriptorTable : Block
{
public:
// ---------------- PUBLIC CONSTANTS ----------------
    static constexpr const uint32_t OFFSET = 2048;

// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    BlockGroupDescriptorTable();
    BlockGroupDescriptorTable(const SuperBlock& sb, uint16_t bg);
    ~BlockGroupDescriptorTable();

    BlockGroupDescriptorTable(const BlockGroupDescriptorTable& table);
    BlockGroupDescriptorTable(BlockGroupDescriptorTable&& table) noexcept;

    BlockGroupDescriptorTable& operator=(const BlockGroupDescriptorTable& table);
    BlockGroupDescriptorTable& operator=(BlockGroupDescriptorTable&& table) noexcept;

// ---------------- PUBLIC METHODS ----------------
    void print_fields() const;

    uint32_t read(const char* file) override;

    uint32_t write(const char* file) const override;

    uint32_t write_bgd(const char* file, uint16_t index) const;

    BlockGroupDescriptor& get_bgd(uint32_t group_num);

    const BlockGroupDescriptor& get_bgd(uint32_t group_num) const;

protected:
// ---------------- PROTECTED VIRTUAL METHODS ----------------
    void* get_fields_buffer_for_read() override { return nullptr; }
    const void* get_fields_buffer_for_write() const override { return nullptr; };

private:
// ---------------- PRIVATE VARIABLES ----------------
    BlockGroupDescriptor* m_table = nullptr;
    uint16_t m_gd_count = 0;

// ---------------- PRIVATE METHODS ----------------
    void free();
    void copy_from(const BlockGroupDescriptorTable& table);
    void move_from(BlockGroupDescriptorTable&& table);
};
