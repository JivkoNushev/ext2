#include <iostream>
#include <cstring>
#include <utility>

#include "FileSystem.h"

FileSystem::FileSystem() {}

FileSystem::FileSystem(FSType type, const char* device_path) : 
    m_type(type)
{
    this->m_device_path = new char[std::strlen(device_path) + 1];
    if(!device_path) return;

    std::strcpy(this->m_device_path, device_path);
}

FileSystem::~FileSystem()
{
    this->_free();

    this->m_device_path = nullptr;
}

FileSystem::FileSystem(const FileSystem& fs)
{
    this->_copy_from(fs);
}

FileSystem::FileSystem(FileSystem&& fs)
{
    this->_move_from(std::move(fs));
}

FileSystem& FileSystem::operator=(const FileSystem& fs)
{
    if(this != &fs)
    {
        this->_free();
        this->_copy_from(fs);
    }

    return *this;
}

FileSystem& FileSystem::operator=(FileSystem&& fs)
{
    if(this != &fs)
    {
        this->_free();
        this->_move_from(std::move(fs));
    }

    return *this;
}

const char* FileSystem::get_device_path() const
{
    return this->m_device_path;
}

void FileSystem::_free()
{
    delete[] this->m_device_path;
}

void FileSystem::_copy_from(const FileSystem& fs)
{
    this->m_type = fs.m_type;
    this->m_device_path = new char[std::strlen(fs.m_device_path) + 1];
    if(!this->m_device_path) return;

    std::strcpy(this->m_device_path, fs.m_device_path);
}

void FileSystem::_move_from(FileSystem&& fs)
{
    this->m_type = fs.m_type;
    this->m_device_path = fs.m_device_path;

    fs.m_device_path = nullptr;
}
