#include <exception>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <string>

#include "ClientInterface.h"
#include "FileSystem.h"
#include "ext2/Ext2.h"

ClientInterface::ClientInterface(int argc, char** argv)
{
    FileSystem::FSType fs_type = FileSystem::FSType::ext2;
    char* file_path = nullptr;

    for(int i = 1; i < argc; i++)
    {
        if(0 == std::strcmp(argv[i], "-h") || 0 == std::strcmp(argv[i], "--help"))
        {
            // this->print_help();
            return;
        }
        else if(0 == std::strcmp(argv[i], "-t") || 0 == std::strcmp(argv[i], "--type"))
        {
            if(++i >= argc) throw std::invalid_argument("[Error] No File System Provided");

            if(0 == std::strcmp(argv[i], "ext2"))
            {
                continue;
            }
            else if(0 == std::strcmp(argv[i], "exFAT"))
            {
                fs_type = FileSystem::FSType::exFAT;
            }
            else
            {
                // this->print_usage_t();
                throw std::invalid_argument("[Error] Invalid File System type");
            }
        }
        else if(0 == std::strcmp(argv[i], "-f") || 0 == std::strcmp(argv[i], "--file"))
        {
            if(++i >= argc) throw std::invalid_argument("[Error] No File Path provided");

            if(!std::ifstream(argv[i]).good()) throw std::invalid_argument("[Error] Invalid File Path");

            file_path = new char[std::strlen(argv[i] + 1)];
            if(!file_path) throw std::runtime_error("[Error] Insufficient Memory");
            std::strcpy(file_path, argv[i]);
        }
    }

    if(!file_path)
    {
        // this->print_usage_f();
        throw std::invalid_argument("[Error] No FS device file provided");
    }

    switch (fs_type) 
    {
        case FileSystem::FSType::ext2:
            this->m_fs = Ext2(std::move(file_path));
            break;
        case FileSystem::FSType::exFAT:
            std::cout << "Not implemented\n";
            this->m_running = false;
            break;
        default:
            std::cout << "Invalid FS Type\n";
            this->m_running = false;
            break;
    }

    this->m_running = true;
}


void ClientInterface::run()
{
    while(this->m_running)
    {
        std::cin.getline(this->m_buffer, 1024);

        if(0 == std::strcmp(this->m_buffer, "exit"))
        {
            this->m_running = false;
        }
    }
}
