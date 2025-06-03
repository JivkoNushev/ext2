#include <utility>

#include "utils/cstring.h"
#include "FileSystem.h"

FileSystem::FileSystem() {}

FileSystem::FileSystem(const char *device_path, FSType type, bool format)
    : m_type(type), m_format(format)
{
    this->m_device_path = new char[utils::strlen(device_path) + 1];
    utils::strcpy(this->m_device_path, device_path);
}

FileSystem::~FileSystem()
{
    this->free();
    this->m_device_path = nullptr;
}

FileSystem::FileSystem(const FileSystem &fs)
{
    this->copy_from(fs);
}

FileSystem::FileSystem(FileSystem &&fs) noexcept
{
    this->move_from(std::move(fs));
}

FileSystem &FileSystem::operator=(const FileSystem &fs)
{
    if (this != &fs)
    {
        this->free();
        this->copy_from(fs);
    }

    return *this;
}

FileSystem &FileSystem::operator=(FileSystem &&fs) noexcept
{
    if (this != &fs)
    {
        this->free();
        this->move_from(std::move(fs));
    }

    return *this;
}

const char *FileSystem::get_device_path() const
{
    return this->m_device_path;
}

void FileSystem::free()
{
    delete[] this->m_device_path;
}

void FileSystem::copy_from(const FileSystem &fs)
{
    this->m_type = fs.m_type;

    this->m_device_path = new char[utils::strlen(fs.m_device_path) + 1];
    utils::strcpy(this->m_device_path, fs.m_device_path);
}

void FileSystem::move_from(FileSystem &&fs)
{
    this->m_type = fs.m_type;
    this->m_device_path = fs.m_device_path;

    fs.m_device_path = nullptr;
}
