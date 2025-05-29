#include <exception>
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

    // this->m_sb.print_fields();
    // this->m_bgdt.print_fields();
    // this->m_it.print_fields();

    // if(!this->create_file("dir1/dir_test", true)) std::cout << "Couldn't create dir\n";
    // if(!this->create_file("dir1/dir_test/test_file", false)) std::cout << "Couldn't create file\n";
    // if(!this->remove_file("dir1/dir_test", true)) std::cout << "Couldn't delete folder\n";
    this->print_tree(2);
}

void Ext2::format_ext2() const
{
    throw std::runtime_error("[Error] Ext2::format_ext2 - not implemented");
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

void Ext2::tree(const char* path) const
{
    if(path == nullptr) return;

    try
    {
        uint32_t start_inode_num = 0;
        this->resolve_path(path, start_inode_num);

        std::cout << path << '\n';
        this->print_tree(start_inode_num); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] Ext2::tree - tree " << path << " exited with: " << e.what() << '\n';
    }
}

void Ext2::print_tree(uint32_t inode_idx, const utils::string& prefix, bool is_last_sibling) const
{
    const Inode& current_inode_obj = ((InodeTable&)this->m_it).get_inode(inode_idx);
    if (!(current_inode_obj.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR)) return;

    utils::vector<LinkedDirectoryEntry> entries = this->read_dir_entries(current_inode_obj); 

    utils::vector<LinkedDirectoryEntry> children_to_display;
    for (size_t i = 0; i < entries.size(); i++)
    {
        const LinkedDirectoryEntry& entry = entries[i];

        // skip current and parent entries
        if (0 != std::strcmp((const char*)entry.name, ".") && 0 != std::strcmp((const char*)entry.name, ".."))
        {
            children_to_display.push_back(entry);
        }
    }

    for (size_t i = 0; i < children_to_display.size(); i++)
    {
        const LinkedDirectoryEntry& child_entry = children_to_display[i];
        is_last_sibling = (i == children_to_display.size() - 1);

        std::cout << prefix << (is_last_sibling ? "└── " : "├── ");

        for(int j = 0; j < child_entry.name_len; j++)
        {
            std::cout << child_entry.name[j];
        }

        bool is_child_inode_dir = ((InodeTable&)this->m_it).get_inode(child_entry.inode).m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR;

        std::cout << (is_child_inode_dir ? "/\n" : "\n");

        if (is_child_inode_dir )
        {
            utils::string new_prefix = prefix + (is_last_sibling ? "    " : "│   ");
            this->print_tree(child_entry.inode, new_prefix, is_last_sibling);
        }
    }
}

utils::vector<LinkedDirectoryEntry> Ext2::read_dir_entries(const Inode& dir_inode) const
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

        this->read_directory_block(data_block_num, entries, block_size);
    }

    uint32_t single_indirect_block_num = dir_inode.m_fields.i_block[12];
    if (single_indirect_block_num != 0)
    {
        this->read_single_indirect_block(single_indirect_block_num, entries, block_size);
    }

    uint32_t double_indirect_block_num = dir_inode.m_fields.i_block[13];
    if (double_indirect_block_num != 0)
    {
        this->read_double_indirect_block(double_indirect_block_num, entries, block_size);
    }

    uint32_t triple_indirect_block_num = dir_inode.m_fields.i_block[14];
    if (triple_indirect_block_num != 0)
    {
        this->read_triple_indirect_block(triple_indirect_block_num, entries, block_size);
    }

    return entries;
}

void Ext2::read_directory_block(uint32_t data_block_num, utils::vector<LinkedDirectoryEntry>& entries, uint32_t block_size) const
{
    uint8_t* block_data = new uint8_t[block_size];

    try
    {
        this->read_block(data_block_num, block_data, block_size);
    }
    catch (const std::exception& e)
    {
        delete[] block_data;
        std::cerr << "[Error] Ext2::read_directory_block - couldn't read block\n";
        throw;
    }

    uint32_t current_offset = 0;
    while (current_offset < block_size)
    {
        LinkedDirectoryEntry* disk_entry = (LinkedDirectoryEntry*)(block_data + current_offset);

        if (disk_entry->rec_len == 0) break;

        if (disk_entry->inode != 0) // not empty/deleted entry
        {
            LinkedDirectoryEntry parsed_entry;
            parsed_entry.inode = disk_entry->inode;
            parsed_entry.rec_len = disk_entry->rec_len;
            parsed_entry.name_len = disk_entry->name_len;
            parsed_entry.file_type = disk_entry->file_type;

            uint8_t size = std::min(disk_entry->name_len, (uint8_t)(DirectoryEntry::MAX_NAME_LEN - 1));
            std::memcpy(parsed_entry.name, disk_entry->name, size);
            parsed_entry.name[size] = '\0';

            entries.push_back(parsed_entry);
        }

        current_offset += disk_entry->rec_len;
    }

    delete[] block_data;
}

utils::vector<uint32_t> Ext2::read_indirect_block_pointers(uint32_t indirect_block_num, uint32_t block_size) const
{
    utils::vector<uint32_t> pointers;

    uint8_t* indirect_block_data = new uint8_t[block_size];

    try
    {
        this->read_block(indirect_block_num, indirect_block_data, block_size);
    }
    catch (const std::exception& e)
    {
        delete[] indirect_block_data;
        std::cerr << "[Error] Ext2::read_indirect_block_pointers - couldn't read block\n";
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

void Ext2::read_single_indirect_block(uint32_t indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size) const
{
    utils::vector<uint32_t> data_block_pointers = read_indirect_block_pointers(indirect_block_num, block_size);

    for (size_t i = 0; i < data_block_pointers.size(); i++)
    {
        read_directory_block(data_block_pointers[i], all_entries, block_size);
    }
}

void Ext2::read_double_indirect_block(uint32_t double_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size) const
{
    utils::vector<uint32_t> single_indirect_block_pointers = read_indirect_block_pointers(double_indirect_block_num, block_size);

    for (size_t i = 0; i < single_indirect_block_pointers.size(); i++)
    {
        read_single_indirect_block(single_indirect_block_pointers[i], all_entries, block_size);
    }
}

void Ext2::read_triple_indirect_block(uint32_t triple_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries, uint32_t block_size) const
{
    utils::vector<uint32_t> double_indirect_block_pointers = read_indirect_block_pointers(triple_indirect_block_num, block_size);

    for (size_t i = 0; i < double_indirect_block_pointers.size(); i++)
    {
        read_double_indirect_block(double_indirect_block_pointers[i], all_entries, block_size);
    }
}

uint32_t Ext2::get_entry_with_name(const utils::vector<LinkedDirectoryEntry> entries, utils::string component_name)
{
    for (size_t i = 0; i < entries.size(); i++)
    {
        const LinkedDirectoryEntry& entry = entries[i];

        if (component_name.size() == entry.name_len &&
            0 == std::strncmp(component_name.c_str(), (const char*)entry.name, entry.name_len))
        {
            return entry.inode;
        }
    }

    return 0;
}

const Inode& Ext2::resolve_path(const utils::string& path, uint32_t& out_inode_num) const
{
    uint32_t current_inode_num = Inode::Reserved::EXT2_ROOT_INO;
    const Inode* current_inode = &((InodeTable&)this->m_it).get_inode(current_inode_num);

    if (path.empty() || path == "/")
    {
        out_inode_num = current_inode_num;
        return *current_inode;
    }

    utils::vector<utils::string> components = utils::split_path(path);

    for (size_t i = 0; i < components.size(); i++)
    {
        const utils::string& component_name = components[i];

        if (!(current_inode->m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR))
        {
            throw std::runtime_error("Ext::resolve_path - component is not a directory.");
        }

        utils::vector<LinkedDirectoryEntry> entries = read_dir_entries(*current_inode);

        current_inode_num = this->get_entry_with_name(entries, component_name);
        if(0 == current_inode_num)
        {
            throw std::runtime_error("Ext::resolve_path - component not found.");
        }

        current_inode = &((InodeTable&)this->m_it).get_inode(current_inode_num);
    }

    out_inode_num = current_inode_num;
    return *current_inode;
}

void Ext2::read_block(uint32_t block_number, uint8_t* buffer, uint32_t block_size) const
{
    std::ifstream ifs(this->get_device_path(), std::ios::binary);

    if (!ifs.is_open())
    {
        throw std::runtime_error("Ext2::read_block - failed to open device");
    }

    uint32_t offset = block_number * block_size;
    ifs.seekg(offset, std::ios_base::beg);
    if (ifs.fail())
    {
         ifs.close();
         throw std::runtime_error("Ext2::read_block - failed to seek");
    }

    ifs.read((char*)(buffer), block_size);
    if (ifs.fail())
    {
         ifs.close();
         throw std::runtime_error("Ext2::read_block - failed to read data");
    }

    ifs.close();
}

void Ext2::write_block(uint32_t block_number, uint8_t* buffer, uint32_t block_size) const
{
    std::ofstream ofs(this->get_device_path(), std::ios::binary | std::ios::in | std::ios::out);

    if (!ofs.is_open())
    {
        throw std::runtime_error("Ext2::write_block - failed to open device");
    }


    uint32_t offset = block_number * block_size;
    ofs.seekp(offset, std::ios_base::beg);
    if (ofs.fail())
    {
         ofs.close();
         throw std::runtime_error("Ext2::write_block - failed to seek");
    }

    ofs.write((const char*)(buffer), block_size);
    if (ofs.fail())
    {
         ofs.close();
         throw std::runtime_error("Ext2::write_block - failed to write data");
    }

    ofs.flush();
    ofs.close();
}
