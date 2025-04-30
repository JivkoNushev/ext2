#include "Disk.h"
#include <filesystem>
#include <fstream>


Disk::Disk(const std::string& disk_img) : disk_img(disk_img) {}

bool Disk::is_empty() const
{
    if(!std::fstream(this->disk_img).good()) return true;

    return std::filesystem::is_empty(this->disk_img);
}


uint32_t Disk::seek(uint32_t position, uint8_t from)
{
    this->position = position;

    return position;
}


uint32_t Disk::read(uint8_t* buffer, uint32_t count)
{
    std::ifstream ifs(this->disk_img);
    ifs.seekg(position, std::ios::beg);
    ifs.read((char*)buffer, count);
    this->position = ifs.tellg();

    ifs.close();

    return this->position;

}
uint32_t Disk::write(const uint8_t* buffer, uint32_t count)
{
    std::ofstream ofs(this->disk_img, std::ios::trunc);
    ofs.seekp(position, std::ios::beg);
    ofs.write((char*)buffer, count);
    this->position = ofs.tellp();
    ofs.close();

    return this->position;
}
