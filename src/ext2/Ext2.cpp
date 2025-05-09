#include <iostream>

#include "Ext2.h"
#include "SuperBlock.h"


Ext2::Ext2(const char* device_path) :
    FileSystem(FSType::ext2, device_path)
{
    std::cout << "Ext2 loaded from: " << this->get_device_path() << '\n';
    this->sb.read(device_path);
}


