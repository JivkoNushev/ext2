#pragma once

#include <cstdint>

class FileSystem
{
public:
    enum class FSType : uint8_t { ext2, exFAT };

public:
    FileSystem();
    FileSystem(FSType type, const char* device_path);
    virtual ~FileSystem();

    FileSystem(const FileSystem& fs);
    FileSystem(FileSystem&& fs);

    FileSystem& operator=(const FileSystem& fs);
    FileSystem& operator=(FileSystem&& fs);

    const char* get_device_path() const;
private:
    void _free();
    void _copy_from(const FileSystem& fs);
    void _move_from(FileSystem&& fs);

    FSType m_type;
    char* m_device_path = nullptr;
};
