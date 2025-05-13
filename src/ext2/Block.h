#pragma once

#include <cstdint>

class Block
{
public:
    Block(uint32_t size, uint32_t offset);
    virtual ~Block() = default;

    virtual uint32_t read(const char* file) = 0;
    virtual uint32_t write(const char* file) const = 0;

public:
    uint32_t m_size = 0;
    uint32_t m_offset = 0;

};
