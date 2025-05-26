#pragma once

#include <cstdint>

class Block
{
public:
// ---------------- PUBLIC VARIABLES ----------------
    uint32_t m_size = 0;
    uint32_t m_offset = 0;

// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    Block(uint32_t size, uint32_t offset);
    virtual ~Block() = default;

// ---------------- PUBLIC METHODS ----------------
    virtual uint32_t read(const char* file);
    virtual uint32_t write(const char* file) const;

protected:
// ---------------- PROTECTED PURE VIRTUAL METHODS ----------------
    virtual void* get_fields_buffer_for_read() = 0;
    virtual const void* get_fields_buffer_for_write() const = 0;
};
