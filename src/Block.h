#pragma once

#include "config.h"

struct Block
{
    uint32_t size = BLOCK_SIZE;
    uint32_t offset = 0;
};
