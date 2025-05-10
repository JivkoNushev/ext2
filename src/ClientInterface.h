#pragma once

#include "FileSystem.h"

class ClientInterface
{
public:
    ClientInterface(int argc, char** argv);
    void run();
private:
    static constexpr uint32_t BUFFER_SIZE = 1024;

private:
    void print_usage_d() const;
    void print_usage_f() const;
    void print_usage_h() const;
    void print_usage_t() const;

    bool m_running = false;
    FileSystem m_fs;
    char m_buffer[BUFFER_SIZE]{};
};
