#include <iostream>
#include <fstream>

#include "Block.h"

Block::Block(uint32_t size, uint32_t offset) :
    m_size(size), m_offset(offset)
{}

uint32_t Block::read(const char* file_name)
{
    void* fields_buffer = this->get_fields_buffer_for_read();
    if (!fields_buffer)
    {
        std::cerr << "[Error] Derived class provided null buffer for read.\n";
        throw std::runtime_error("Block::read - Null buffer from derived class");
    }
    if (0 == this->m_size)
    {
        std::cerr << "[Error] Block size is 0\n";
        throw std::runtime_error("Block::read - Block size is 0 in Block::read");
    }

    std::ifstream ifs(file_name, std::ios::binary);
    if (!ifs.is_open())
    {
        std::cerr << "[Error] Failed to open device " << file_name << '\n';
        throw std::runtime_error("Block::read - Failed to open device for Block::read");
    }

    ifs.seekg(this->m_offset, std::ios_base::beg);
    if (ifs.fail())
    {
        std::cerr << "[Error] Failed to seek to offset " << this->m_offset << '\n';
        ifs.close();
        throw std::runtime_error("Block::read - Failed to seek in Block::read");
    }

    ifs.read((char*)(fields_buffer), this->m_size);

    if (ifs.fail())
    {
        std::cerr << "[Error] Failed to read data from file " << file_name << '\n';
        ifs.close();
        throw std::runtime_error("Block::read - Failed to read data or incorrect amount in Block::read");
    }

    ifs.close();
    return this->m_size;
}

uint32_t Block::write(const char* file_name) const
{
    const void* fields_buffer = this->get_fields_buffer_for_write();
    if (!fields_buffer)
    {
        std::cerr << "[Error] Derived class provided null buffer for write.\n";
        throw std::runtime_error("Block::write - Null buffer in Block::write from derived class");
    }
    if (0 == this->m_size) 
    {
        std::cerr << "[Error] Block size is 0\n";
        throw std::runtime_error("Block::write - Block size is 0 in Block::write");
    }

    std::fstream fs(file_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!fs.is_open())
    {
        std::cerr << "[Error] Failed to open device " << file_name << '\n';
        throw std::runtime_error("Block::write - Failed to open device for Block::write");
    }

    fs.seekp(this->m_offset, std::ios_base::beg);
    if (fs.fail())
    {
        std::cerr << "[Error] Failed to seek to offset " << this->m_offset << '\n';
        fs.close();
        throw std::runtime_error("Block::write - Failed to seek in Block::write");
    }

    fs.write((const char*)(fields_buffer), this->m_size);
    if (fs.fail())
    {
        std::cerr << "[Error] Failed to write data to file " << file_name << '\n';
        fs.close();
        throw std::runtime_error("Block::write - Failed to write data in Block::write");
    }

    fs.flush();
    fs.close();
    return this->m_size;
}
