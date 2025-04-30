#pragma once

#include <cstdint>
#include <iostream>

class Disk
{
public:
    static const uint8_t Begin = std::ios::beg;

    Disk(const std::string& disk_img);

    uint32_t seek(uint32_t position, uint8_t from);
    uint32_t read(uint8_t* buffer, uint32_t count);
    uint32_t write(const uint8_t* buffer, uint32_t count);

    bool is_empty() const;

private:
    std::string disk_img;

    uint32_t position = 0;

};
