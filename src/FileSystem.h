#pragma once

#include <cstdint>

class FileSystem
{
public:
// ---------------- PUBLIC TYPES ----------------
    enum class FSType : uint8_t
    {
        ext2,
        exFAT
    };

// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    FileSystem();
    FileSystem(const char* device_path, FSType type, bool format);
    virtual ~FileSystem();

    FileSystem(const FileSystem& fs);
    FileSystem(FileSystem&& fs) noexcept;

    FileSystem& operator=(const FileSystem& fs);
    FileSystem& operator=(FileSystem&& fs) noexcept;

// ---------------- PUBLIC METHODS ----------------
    const char* get_device_path() const;


// ---------------- PUBLIC VIRTUAL METHODS ----------------
    virtual void tree(const char* path) const noexcept = 0;
    virtual void cat(const char* path) const noexcept = 0;
    virtual void write(const char* path, const char* data, bool append) noexcept = 0;
    virtual void touch(const char* path) noexcept = 0;
    virtual void mkdir(const char* path) noexcept = 0;
    virtual void rm(const char* path, bool recursive = false) noexcept = 0;

private:
// ---------------- PRIVATE VARIABLES ----------------
    char* m_device_path = nullptr;
    FSType m_type;
    bool m_format = false;

// ---------------- PRIVATE METHODS ----------------
    void _free();
    void _copy_from(const FileSystem& fs);
    void _move_from(FileSystem&& fs);
};
