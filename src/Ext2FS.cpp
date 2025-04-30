#include "Ext2FS.h"
#include "Disk.h"

Ext2FS::Ext2FS(const std::string& disk_img) :
    disk(disk_img),
    sb()
{
    if(this->disk.is_empty())
    {
        std::cout << "[Log] Formating new ext2 fs.\n";
        this->format();
        this->save();
    }
    else
    {
        std::cout << "[Log] Loading ext2 fs.\n";
        this->load();
    }
}

void Ext2FS::load()
{
    this->sb.load(this->disk);
}

void Ext2FS::format()
{
    this->sb.format();
}

void Ext2FS::save()
{
    this->sb.save(this->disk);
}


