#include <iostream>
#include <stdexcept>

#include "config.h"
#include "Ext2FS.h"
#include "ClientInterface.h"


int main()
{
    try
    {
        Ext2FS fs(FS_PATH);
        // ClientInterface cli(std::move(fs));

        // cli.run();
    }
    catch (std::invalid_argument e)
    {
        std::cerr << "[Error]: " << e.what() << '\n';
    }

    return 0;
}
