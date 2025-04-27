#pragma once

#include <cstdint>

constexpr uint16_t BLOCK_SIZE = 1024;

struct Block
{
    uint8_t data[BLOCK_SIZE]{};
};
