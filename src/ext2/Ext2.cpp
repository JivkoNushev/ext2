#include <iostream>
#include <stdexcept>

#include "Ext2.h"
#include "SuperBlock.h"


Ext2::Ext2(const char* device_path, bool format) :
    FileSystem(device_path, FSType::ext2, format)
{
    if(format)
    {
        throw std::invalid_argument("[Error] Formatting ext2 is not implemented");
    }

    std::cout << "Loading ext2 from: " << this->get_device_path() << '\n';

    this->sb.read(device_path);
}


