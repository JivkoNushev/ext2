#include <iostream>
#include <stdexcept>
#include <cstring>
#include <fstream>

#include "Ext2.h"
#include "BlockGroupDescriptor.h"
#include "BlockGroupDescriptorTable.h"
#include "DirectoryEntry.h"
#include "SuperBlock.h"


Ext2::Ext2(const char* device_path, bool format) :
    FileSystem(device_path, FSType::ext2, format),
    m_it(this->m_sb, this->m_bgdt, this->get_device_path())
{
    if(format)
    {
        this->format_ext2();
    }
    else
    {
        this->load_ext2();
    }

    this->m_sb.print_fields();
    this->m_bgdt.print_fields();
    // this->m_it.print_fields();

    // if(!this->create_file("dir1/dir_test", true)) std::cout << "Couldn't create dir\n";
    // if(!this->create_file("dir1/dir_test/test_file", false)) std::cout << "Couldn't create file\n";
    // if(!this->remove_file("dir1/dir_test", true)) std::cout << "Couldn't delete folder\n";
    this->print_tree(2);
}

void Ext2::format_ext2() const
{
    throw std::runtime_error("[Error] Formatting ext2 is not implemented");
}

void Ext2::load_ext2()
{
    std::cout << "Loading ext2 from: " << this->get_device_path() << '\n';
    this->m_sb = SuperBlock(this->get_device_path());

    this->m_bgdt = BlockGroupDescriptorTable(this->m_sb, 0);
    this->m_bgdt.read(this->get_device_path());

    this->m_it = InodeTable(this->m_sb, this->m_bgdt, this->get_device_path());
    this->m_it.read(get_device_path());
}

void Ext2::tree(const char* path)
{
    if(path == nullptr) return;

    try
    {
        uint32_t start_inode_num = 0;
        this->resolve_path(path, start_inode_num); 
        this->print_tree(start_inode_num); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] Couldn't tree " << path << ": " << e.what() << '\n';
    }
}

void Ext2::print_tree(uint32_t inode_idx, const utils::string& prefix, bool is_last_sibling)
{
    Inode& current_inode_obj = this->m_it.get_inode(inode_idx);
    if (!(current_inode_obj.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR)) return;

    if (inode_idx == Inode::Reserved::EXT2_ROOT_INO && prefix.empty())
    {
        std::cout << "/ (inode " << inode_idx << ")\n";
    }

    utils::vector<LinkedDirectoryEntry> entries = this->read_dir_entries(current_inode_obj); 

    utils::vector<LinkedDirectoryEntry> children_to_display;
    for (size_t i = 0; i < entries.size(); i++)
    {
        const LinkedDirectoryEntry& entry = entries[i];

        if (entry.inode == 0) continue;

        if (0 == std::strcmp((const char*)entry.name, ".") || 0 == std::strcmp((const char*)entry.name, "..")) 
            continue;

        children_to_display.push_back(entry);
    }

    for (size_t i = 0; i < children_to_display.size(); i++)
    {
        const LinkedDirectoryEntry& child_entry = children_to_display[i];
        is_last_sibling = (i == children_to_display.size() - 1);

        std::cout << prefix << (is_last_sibling ? "└── " : "├── ");

        for(int k = 0; k < child_entry.name_len; k++)
        {
            std::cout << child_entry.name[k];
        }

        Inode child_inode_obj_for_type_check = m_it.get_inode(child_entry.inode);
        bool child_is_dir = false;
        if (child_inode_obj_for_type_check.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR)
        {
            std::cout << "/";
            child_is_dir = true;
        }
        std::cout << " (inode " << child_entry.inode << ")\n";

        if (child_is_dir)
        {
            utils::string new_prefix = prefix + (is_last_sibling ? "    " : "│   ");
            print_tree(child_entry.inode, new_prefix, is_last_sibling);
        }
    }
}

utils::vector<LinkedDirectoryEntry> Ext2::read_dir_entries(const Inode& dir_inode)
{
    utils::vector<LinkedDirectoryEntry> entries;
    uint32_t block_size = m_sb.get_block_size();

    if (!(dir_inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR))
    {
        return entries;
    }

    for (int i = 0; i < 12; i++)
    {
        uint32_t data_block_num = dir_inode.m_fields.i_block[i];
        if (data_block_num == 0) continue;
        read_directory_block(data_block_num, entries, block_size);
    }

    uint32_t single_indirect_block_num = dir_inode.m_fields.i_block[12];
    if (single_indirect_block_num != 0)
    {
        read_single_indirect_block(single_indirect_block_num, entries, block_size);
    }

    uint32_t double_indirect_block_num = dir_inode.m_fields.i_block[13];
    if (double_indirect_block_num != 0)
    {
        read_double_indirect_block(double_indirect_block_num, entries, block_size);
    }

    uint32_t triple_indirect_block_num = dir_inode.m_fields.i_block[14];
    if (triple_indirect_block_num != 0)
    {
        read_triple_indirect_block(triple_indirect_block_num, entries, block_size);
    }

    return entries;
}

void Ext2::read_directory_block(uint32_t data_block_num, utils::vector<LinkedDirectoryEntry>& entries, uint32_t block_size)
{
    uint8_t* block_buffer = new uint8_t[block_size];

    try
    {
        read_block(data_block_num, block_buffer, block_size);
    }
    catch (const std::exception& e)
    {
        delete[] block_buffer;
        throw;
    }

    uint32_t current_offset = 0;
    while (current_offset < block_size)
    {
        LinkedDirectoryEntry* disk_entry = reinterpret_cast<LinkedDirectoryEntry*>(block_buffer + current_offset);

        if (disk_entry->rec_len == 0) break;

        if (disk_entry->rec_len < (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t)) ||
            current_offset + disk_entry->rec_len > block_size)
        {
            std::cerr << "[Error] Invalid rec_len: " << disk_entry->rec_len << '\n';
            throw std::runtime_error("Ext2::read_directory_block - invalid rec_len");
        }

        if (disk_entry->inode != 0)
        {
            LinkedDirectoryEntry new_parsed_entry;
            new_parsed_entry.inode = disk_entry->inode;
            new_parsed_entry.rec_len = disk_entry->rec_len;
            new_parsed_entry.name_len = disk_entry->name_len;
            new_parsed_entry.file_type = disk_entry->file_type;

            uint8_t len_to_copy = std::min(disk_entry->name_len, (uint8_t)(sizeof(new_parsed_entry.name) - 1));
            std::memcpy(new_parsed_entry.name, disk_entry->name, len_to_copy);
            new_parsed_entry.name[len_to_copy] = '\0';

            entries.push_back(new_parsed_entry);
        }

        current_offset += disk_entry->rec_len;
    }

    delete[] block_buffer;
}

utils::vector<uint32_t> Ext2::read_indirect_block_pointers(uint32_t indirect_block_num, uint32_t block_size)
{
    utils::vector<uint32_t> pointers;
    if (indirect_block_num == 0) return pointers;

    uint8_t* indirect_block_data = new uint8_t[block_size];

    try
    {
        read_block(indirect_block_num, indirect_block_data, block_size);
    }
    catch (const std::exception& e)
    {
        delete[] indirect_block_data;
        throw;
    }

    uint32_t num_pointers_in_block = block_size / sizeof(uint32_t);
    uint32_t* block_ptr_array = (uint32_t*)(indirect_block_data);

    for (uint32_t i = 0; i < num_pointers_in_block; i++)
    {
        if (block_ptr_array[i] == 0) continue;

        pointers.push_back(block_ptr_array[i]);
    }

    delete[] indirect_block_data;
    return pointers;
}

void Ext2::read_single_indirect_block(uint32_t indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size)
{
    utils::vector<uint32_t> data_block_pointers = read_indirect_block_pointers(indirect_block_num, block_size);

    for (size_t i = 0; i < data_block_pointers.size(); i++)
    {
        uint32_t data_block_num = data_block_pointers[i];

        if (data_block_num == 0) continue;
        read_directory_block(data_block_num, all_entries, block_size);
    }
}

void Ext2::read_double_indirect_block(uint32_t double_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size)
{
    utils::vector<uint32_t> single_indirect_block_pointers = read_indirect_block_pointers(double_indirect_block_num, block_size);

    for (size_t i = 0; i < single_indirect_block_pointers.size(); i++)
    {
        uint32_t single_indirect_block_ptr = single_indirect_block_pointers[i];

        if (single_indirect_block_ptr == 0) continue;
        read_single_indirect_block(single_indirect_block_ptr, all_entries, block_size);
    }
}

void Ext2::read_triple_indirect_block(uint32_t triple_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size)
{
    utils::vector<uint32_t> double_indirect_block_pointers = read_indirect_block_pointers(triple_indirect_block_num, block_size);

    for (size_t i = 0; i < double_indirect_block_pointers.size(); i++)
    {
        uint32_t double_indirect_block_ptr = double_indirect_block_pointers[i];
        if (double_indirect_block_ptr == 0) continue;
        read_double_indirect_block(double_indirect_block_ptr, all_entries, block_size);
    }
}

Inode Ext2::resolve_path(const utils::string& path, uint32_t& out_inode_num)
{
    uint32_t current_inode_num = Inode::Reserved::EXT2_ROOT_INO;
    Inode current_inode = m_it.get_inode(current_inode_num);

    if (path.empty() || path == "/")
    {
        out_inode_num = current_inode_num;
        return current_inode;
    }

    utils::vector<utils::string> components = utils::split_path(path);
    if (components.empty() && path != "/") // something like "///"
    {
        out_inode_num = current_inode_num;
        return current_inode;
    }


    for (size_t i = 0; i < components.size(); i++)
    {
        const utils::string& component_name = components[i];

        if (!(current_inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR))
        {
            std::cerr << "[Error] Path component (inode " << current_inode_num << ") is not a directory.\n";
            throw std::runtime_error("Ext::resolve_path - component is not a directory.");
        }

        utils::vector<LinkedDirectoryEntry> entries = read_dir_entries(current_inode);


        bool found_component = false;
        for (size_t i = 0; i < entries.size(); i++)
        {
            const LinkedDirectoryEntry& entry = entries[i];
            if (entry.inode != 0 && component_name.size() == entry.name_len && \
                std::strncmp(component_name.c_str(), (const char*)entry.name, entry.name_len) == 0)
            {
                current_inode_num = entry.inode;
                current_inode = m_it.get_inode(current_inode_num);
                found_component = true;
                break;
            }
        }

        if (!found_component)
        {
            throw std::runtime_error("Ext::resolve_path - component not found.");
        }
    }

    out_inode_num = current_inode_num;
    return current_inode;
}

void Ext2::read_block(uint32_t block_number, uint8_t* buffer, uint32_t block_size) const
{
    std::ifstream ifs(this->get_device_path(), std::ios::binary);

    if (!ifs.is_open())
    {
        std::cerr << "[Error] Failed to open device path '" << this->get_device_path() << "' for reading block " << block_number << '\n';
        throw std::runtime_error("Ext2::read_block - failed to open device");
    }

    uint32_t offset = block_number * block_size;
    ifs.seekg(offset, std::ios_base::beg);
    if (ifs.fail())
    {
         std::cerr << "[Error] Failed to seek to offset " << offset << '\n';
         ifs.close();
         throw std::runtime_error("Ext2::read_block - failed to seek");
    }

    ifs.read((char*)(buffer), block_size);
    if (ifs.fail())
    {
         std::cerr << "[Error] Failed to read a block from file device\n";
         ifs.close();
         throw std::runtime_error("Ext2::read_block - failed to read data");
    }

    ifs.close();
}
