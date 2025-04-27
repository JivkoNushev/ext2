#include <exception>
#include <iostream>

#include "ClientInterface.h"
#include "Ext2FS.h"
#include "utils.h"

ClientInterface::ClientInterface(Ext2FS&& fs) : fs(std::move(fs))
{
    this->is_running = true;
}

void ClientInterface::run()
{
    this->print_help();

    while(this->is_running)
    {
        try
        {
            std::string input = this->_get_input();

            std::string command = str_pop_word(input, ' ');
            if(0 == command.compare("ls"))
            {
                this->fs.list_directory(input);
            }
        }
        catch(std::exception e)
        {
            std::cerr << "[Error]: " << e.what() << '\n';
            this->is_running = false;
        }
    }
}
