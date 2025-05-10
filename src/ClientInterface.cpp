#include <iostream>
#include <fstream>
#include <cstring>

#include "ClientInterface.h"
#include "FileSystem.h"
#include "ext2/Ext2.h"

ClientInterface::ClientInterface(int argc, char** argv)
{
    char* file_path = nullptr;
    FileSystem::FSType fs_type = FileSystem::FSType::ext2;
    bool format = false;

    for(int i = 1; i < argc; i++)
    {
        if(0 == std::strcmp(argv[i], "-h") || 0 == std::strcmp(argv[i], "--help"))
        {
            this->print_usage_h();
            return;
        }
        else if(0 == std::strcmp(argv[i], "-t") || 0 == std::strcmp(argv[i], "--type"))
        {
            if(++i >= argc)
            {
                this->print_usage_t();
                return;
            }

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
                this->print_usage_t();
                return;
            }
        }
        else if(0 == std::strcmp(argv[i], "-d") || 0 == std::strcmp(argv[i], "--device"))
        {
            if(++i >= argc)
            {
                this->print_usage_d();
                return;
            }

            if(!std::ifstream(argv[i]).good()) throw std::invalid_argument("[Error] Invalid File Path");

            file_path = new char[std::strlen(argv[i] + 1)];
            if(!file_path) throw std::runtime_error("[Error] Insufficient Memory");

            std::strcpy(file_path, argv[i]);
        }
        else if(0 == std::strcmp(argv[i], "-f") || 0 == std::strcmp(argv[i], "--format"))
        {
            format = true;
        }
        else
        {
            this->print_usage_h();
            return;
        }
    }

    if(!file_path)
    {
        this->print_usage_h();
        return;
    }

    switch (fs_type) 
    {
        case FileSystem::FSType::ext2:
            this->m_fs = Ext2(std::move(file_path), format);
            break;
        case FileSystem::FSType::exFAT:
            throw std::invalid_argument("[Error] Not implemented");
        default:
            throw std::invalid_argument("[Error] Invalid File System type");
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

void ClientInterface::print_usage_d() const
{
    std::cout <<
"[USAGE]:\n\
    -d, --device DEVICE_NAME        - choose a device to read/write.\n\
";
}

void ClientInterface::print_usage_f() const
{
    std::cout <<
"[USAGE]:\n\
    -f, --format                    -formats the given device instead of using it\n\
";
}

void ClientInterface::print_usage_h() const
{
    std::cout <<
"[USAGE]:\n\
    ext2 [OPTION]... -f DEVICE_NAME\n\
[OPTIONS]\n\
    -f, --format                    -formats the given device instead of using it\n\
    -t, --type FS_TYPE              - choose a file system FS_TYPE can be: ext2, exFAT.\n\
    -d, --device DEVICE_NAME        - choose a device to read/write.\n\
";
}

void ClientInterface::print_usage_t() const
{
    std::cout <<
"[USAGE]:\n\
    -t, --type FS_TYPE              - choose a file system FS_TYPE can be: ext2, exFAT.\n\
";

}
