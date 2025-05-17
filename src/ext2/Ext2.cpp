#include <iostream>
#include <stdexcept>

#include "Ext2.h"
#include "BlockGroupDescriptorTable.h"
#include "SuperBlock.h"


Ext2::Ext2(const char* device_path, bool format) :
    FileSystem(device_path, FSType::ext2, format)
{
    if(format)
    {
        this->format_ext2();
    }
    else
    {
        this->load_ext2();
    }

    this->m_sb.print_fields();
    this->m_bgdt.print_fields();
    this->m_it.print_fields();
}

void Ext2::format_ext2() const
{
    throw std::invalid_argument("[Error] Formatting ext2 is not implemented");
}

void Ext2::load_ext2()
{
    std::cout << "Loading ext2 from: " << this->get_device_path() << '\n';

    this->m_sb = SuperBlock(SuperBlock::SB_SIZE, SuperBlock::SB_OFFSET),
    this->m_sb.read(this->get_device_path());
    this->m_sb.load();

    this->m_bgdt = BlockGroupDescriptorTable(this->m_sb, 0);
    this->m_bgdt.read(this->get_device_path());

    this->m_it = InodeTable(this->m_sb, this->m_bgdt, 0);
    this->m_it.read(this->get_device_path());
}
