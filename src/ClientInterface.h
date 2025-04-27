#pragma once

#include "Ext2FS.h"

class ClientInterface
{
public:
    ClientInterface(Ext2FS&& fs);

    void run();

private:
    void print_help() const;

    std::string _get_input() const;


    Ext2FS fs;

    bool is_running = false;
};
