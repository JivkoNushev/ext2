#pragma once

#include "../FileSystem.h"
#include "SuperBlock.h"

class Ext2 : public FileSystem
{
public:
    Ext2(const char* device_path, bool format);
    ~Ext2() = default;

private:
    SuperBlock sb;
};
