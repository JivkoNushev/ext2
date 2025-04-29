#pragma once

#include <cstdint>
#include <iostream>

class Disk
{
public:
    static const uint8_t Begin = std::ios::beg;

    uint32_t seek(uint32_t position, uint8_t from);
    uint32_t read(uint8_t* buffer, uint32_t count);
    uint32_t write(const uint8_t* buffer, uint32_t count);

private:

};
