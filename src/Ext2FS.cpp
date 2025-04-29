#include "Ext2FS.h"


Ext2FS::Ext2FS(const std::string& filename)
{
    std::fstream disk_img(filename);
    if(disk_img.good())
    {
        // read contents
    } 
    else
    {
        // format new ext2
    }
}


