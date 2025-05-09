#include <stdexcept>
#include <iostream>

#include "ClientInterface.h"

int main(int argc, char** argv)
{
    try
    {
        ClientInterface(argc, argv).run();
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
