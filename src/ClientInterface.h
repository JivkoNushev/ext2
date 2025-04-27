#pragma once

#include "Ext2FS.h"

class ClientInterface
{
public:
    ClientInterface(Ext2FS&& fs);

    void run();

private:
    Ext2FS fs;
};
