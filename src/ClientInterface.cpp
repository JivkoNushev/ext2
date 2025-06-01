#include <iostream>
#include <fstream>

#include "ClientInterface.h"
#include "FileSystem.h"
#include "ext2/Ext2.h"
#include "utils/utils.h"

ClientInterface::ClientInterface(int argc, char** argv)
{
    char* file_path = nullptr;
    FileSystem::FSType fs_type = FileSystem::FSType::ext2;
    bool format = false;

    for(int i = 1; i < argc; i++)
    {
        if(0 == utils::strcmp(argv[i], "-h") || 0 == utils::strcmp(argv[i], "--help"))
        {
            this->print_usage_h();
            return;
        }
        else if(0 == utils::strcmp(argv[i], "-t") || 0 == utils::strcmp(argv[i], "--type"))
        {
            if(++i >= argc)
            {
                this->print_usage_t();
                return;
            }

            if(0 == utils::strcmp(argv[i], "ext2"))
            {
                continue;
            }
            else if(0 == utils::strcmp(argv[i], "exFAT"))
            {
                fs_type = FileSystem::FSType::exFAT;
            }
            else
            {
                this->print_usage_t();
                return;
            }
        }
        else if(0 == utils::strcmp(argv[i], "-d") || 0 == utils::strcmp(argv[i], "--device"))
        {
            if(++i >= argc)
            {
                this->print_usage_d();
                return;
            }

            if(!std::ifstream(argv[i]).good()) throw std::invalid_argument("[Error] Invalid File Path");

            file_path = new char[utils::strlen(argv[i] + 1)];
            if(!file_path) throw std::runtime_error("[Error] Insufficient Memory");

            utils::strcpy(file_path, argv[i]);
        }
        else if(0 == utils::strcmp(argv[i], "-f") || 0 == utils::strcmp(argv[i], "--format"))
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
            this->m_fs = new Ext2(std::move(file_path), format);
            break;
        case FileSystem::FSType::exFAT:
            throw std::invalid_argument("[Error] Not implemented");
        default:
            throw std::invalid_argument("[Error] Invalid File System type");
    }

    this->m_running = true;
}

ClientInterface::~ClientInterface()
{
    delete this->m_fs;
    this->m_fs = nullptr;
}

void ClientInterface::run()
{
    while(this->m_running)
    {
        std::cin.getline(this->m_buffer, 1024);
        this->m_buffer[1023] = '\0';
        utils::vector<utils::string> words = utils::split_words((const char*)this->m_buffer);
        if(0 == utils::strcmp(this->m_buffer, "exit"))
        {
            this->m_running = false;
        }
        else if(words[0] == "tree")
        {
            this->m_fs->tree(words[1].c_str());
        }
        else if(words[0] == "cat")
        {
            this->m_fs->cat(words[1].c_str());
        }
        else if (words[0] == "touch")
        {
            this->m_fs->touch(words[1].c_str());
        }
        else if (words[0] == "mkdir")
        {
            this->m_fs->mkdir(words[1].c_str());
        }
        else if (words[0] == "rm")
        {
            this->m_fs->rm(words[2].c_str(), (words[1] == "-r") ? true : false);
        }
        else if (words[0] == "write" || words[0] == "append")
        {
            utils::string filepath = words[1];
            bool append_mode = (words[0] == "append");

            char buffer[1 << 16]{};
            std::cin.getline(buffer, 1 << 16);

            this->m_fs->write(filepath.c_str(), buffer, append_mode);
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
