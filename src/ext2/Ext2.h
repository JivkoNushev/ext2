#pragma once

#include "../FileSystem.h"
#include "SuperBlock.h"
#include "BlockGroupDescriptorTable.h"

class Ext2 : public FileSystem
{
public:
    Ext2(const char* device_path, bool format);
    ~Ext2() = default;

private:
    void load_ext2();
    void format_ext2() const;

    SuperBlock m_sb;
    BlockGroupDescriptorTable m_bgdt;
};
