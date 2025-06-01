#pragma once

#include "FileSystem.h"

class ClientInterface
{
public:
    static constexpr uint32_t BUFFER_SIZE = 1024;

    ClientInterface(int argc, char** argv);
    ~ClientInterface();


    void run();

private:
    bool m_running = false;
    FileSystem* m_fs;
    char m_buffer[BUFFER_SIZE]{};

    void print_usage_d() const;
    void print_usage_f() const;
    void print_usage_h() const;
    void print_usage_t() const;
    void print_shell_help() const;
};
