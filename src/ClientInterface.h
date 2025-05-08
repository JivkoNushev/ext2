#pragma once

#include "FileSystem.h"

class ClientInterface
{
public:
    ClientInterface();
    void run();
private:
    static FileSystem::FSType parse_fs_type(const char* buffer);

    static constexpr uint32_t BUFFER_SIZE = 1024;

private:
    bool m_running = false;
    FileSystem m_fs;

    char m_buffer[BUFFER_SIZE]{};
};
