#include <iostream>
#include <exception>

#include "ClientInterface.h"

int main(int argc, char** argv)
{
    try
    {
        ClientInterface(argc, argv).run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
