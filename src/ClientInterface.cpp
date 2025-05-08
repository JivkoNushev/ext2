#include <exception>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <string>

#include "ClientInterface.h"
#include "FileSystem.h"
#include "ext2/Ext2.h"

ClientInterface::ClientInterface() :
    m_running(true)
{
    try
    {
        std::cout << "Enter FS type: ";
        std::cin >> this->m_buffer;

        FileSystem::FSType type = ClientInterface::parse_fs_type(this->m_buffer);

        std::cout << "Enter FS path: ";
        std::cin >> this->m_buffer;

        switch (type) 
        {
            case FileSystem::FSType::Ext2:
                this->m_fs = Ext2(this->m_buffer);
                break;
            case FileSystem::FSType::ExFAT:
                std::cout << "Not implemented\n";
                this->m_running = false;
                break;
            default:
                std::cout << "Invalid FS Type\n";
                this->m_running = false;
                break;
        }
    }
    catch(std::invalid_argument)
    {
        this->m_running = false;
    }
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


FileSystem::FSType ClientInterface::parse_fs_type(const char* buffer)
{
    if(0 == std::strcmp(buffer, "ext2"))
    {
        return FileSystem::FSType::Ext2;
    }
    else if(0 == std::strcmp(buffer, "exfat"))
    {
        return FileSystem::FSType::ExFAT;
    }

    throw std::invalid_argument("Couldn't parse FS type");
}
