#pragma once

#include "BlockGroupDescriptor.h"

class BlockGroupDescriptorTable : Block
{
public:
    BlockGroupDescriptorTable();
    BlockGroupDescriptorTable(uint32_t size, uint32_t offset, uint16_t gd_count);
    ~BlockGroupDescriptorTable();

    BlockGroupDescriptorTable(const BlockGroupDescriptorTable& table);
    BlockGroupDescriptorTable(BlockGroupDescriptorTable&& table) noexcept;

    BlockGroupDescriptorTable& operator=(const BlockGroupDescriptorTable& table);
    BlockGroupDescriptorTable& operator=(BlockGroupDescriptorTable&& table) noexcept;

    uint32_t read(const char* file) override;
    uint32_t write(const char* file) const override;

    void print_fields() const;

public:
    static constexpr const uint16_t BGDT_OFFSET = 2048;

private:
    void free();
    void copy_from(const BlockGroupDescriptorTable& table);
    void move_from(BlockGroupDescriptorTable&& table);

    BlockGroupDescriptor* m_table = nullptr;
    uint16_t m_gd_count = 0;
};
