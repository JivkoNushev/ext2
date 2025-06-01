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

void Ext2::tree(const char* path) const noexcept
{
    if(path == nullptr) return;

    try
    {
        uint32_t start_inode_num = 0;
        ((Ext2*)this)->resolve_path(path, start_inode_num);

        std::cout << path << '\n';
        this->print_tree(start_inode_num); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] Ext2::tree - " << e.what() << '\n';
    }
}

void Ext2::cat(const char* path) const noexcept
{
    if (!path) return;

    try
    {
        const utils::vector<uint8_t> data = ((Ext2*)this)->read_file(path); 
        utils::print_utf8(data);
        std::cout << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] Ext2::cat - " << e.what() << '\n';
    }
}

void Ext2::write(const char* path, const char* data, bool append) noexcept
{
    try
    {
        utils::vector<uint8_t> data_vector((uint8_t*)data, utils::strlen(data));
        this->write_file(path, data_vector, append);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[Error] Ext2::touch - " << e.what() << '\n';
    }
}

void Ext2::touch(const char* path) noexcept
{
    try
    {
        this->create_file(path, false);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[Error] Ext2::touch - " << e.what() << '\n';
    }
}

void Ext2::mkdir(const char* path) noexcept
{
    try
    {
        this->create_file(path, true);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[Error] Ext2::mkdir - " << e.what() << '\n';
    }
}

void Ext2::rm(const char* path, bool recursive) noexcept
{
    try
    {
        this->remove_file(path, recursive);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[Error] Ext2::rm - " << e.what() << '\n';
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

    if (!(dir_inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR))
    {
        return entries;
    }

    for (int i = 0; i < 12; i++)
    {
        uint32_t data_block_num = dir_inode.m_fields.i_block[i];
        if (data_block_num == 0) continue;

        this->read_directory_block(data_block_num, entries);
    }

    uint32_t single_indirect_block_num = dir_inode.m_fields.i_block[12];
    if (single_indirect_block_num != 0)
    {
        this->read_single_indirect_block(single_indirect_block_num, entries);
    }

    uint32_t double_indirect_block_num = dir_inode.m_fields.i_block[13];
    if (double_indirect_block_num != 0)
    {
        this->read_double_indirect_block(double_indirect_block_num, entries);
    }

    uint32_t triple_indirect_block_num = dir_inode.m_fields.i_block[14];
    if (triple_indirect_block_num != 0)
    {
        this->read_triple_indirect_block(triple_indirect_block_num, entries);
    }

    return entries;
}

void Ext2::read_directory_block(uint32_t data_block_num, utils::vector<LinkedDirectoryEntry>& entries) const
{
    uint32_t block_size = this->m_sb.get_block_size();

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

utils::vector<uint32_t> Ext2::read_indirect_block_pointers(uint32_t indirect_block_num) const
{
    uint32_t block_size = this->m_sb.get_block_size();

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

void Ext2::read_single_indirect_block(uint32_t indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries) const
{
    utils::vector<uint32_t> data_block_pointers = this->read_indirect_block_pointers(indirect_block_num);

    for (size_t i = 0; i < data_block_pointers.size(); i++)
    {
        this->read_directory_block(data_block_pointers[i], all_entries);
    }
}

void Ext2::read_double_indirect_block(uint32_t double_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries) const
{
    utils::vector<uint32_t> single_indirect_block_pointers = this->read_indirect_block_pointers(double_indirect_block_num);

    for (size_t i = 0; i < single_indirect_block_pointers.size(); i++)
    {
        this->read_single_indirect_block(single_indirect_block_pointers[i], all_entries);
    }
}

void Ext2::read_triple_indirect_block(uint32_t triple_indirect_block_num, utils::vector<LinkedDirectoryEntry>& all_entries) const
{
    utils::vector<uint32_t> double_indirect_block_pointers = this->read_indirect_block_pointers(triple_indirect_block_num);

    for (size_t i = 0; i < double_indirect_block_pointers.size(); i++)
    {
        this->read_double_indirect_block(double_indirect_block_pointers[i], all_entries);
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

Inode& Ext2::resolve_path(const utils::string& path, uint32_t& out_inode_num)
{
    uint32_t current_inode_num = Inode::Reserved::EXT2_ROOT_INO;
    Inode* current_inode = &this->m_it.get_inode(current_inode_num);

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

        current_inode = &this->m_it.get_inode(current_inode_num);
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


uint16_t Ext2::calculate_rec_len(uint8_t name_length)
{
    uint16_t base_size = sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t);
    return (base_size + name_length + 3) & ~3;
}


void Ext2::check_entry_name_validity(const utils::string& name)
{
    if (name.empty() || name.size() > DirectoryEntry::MAX_NAME_LEN) {
        throw std::runtime_error("Ext2::check_entry_name_validity - Name is empty or too long.");
    }

    if (name == "." || name == "..") {
        throw std::runtime_error("Ext2::check_entry_name_validity - Name cannot be '.' or '..'.");
    }
}

uint32_t Ext2::allocate(uint32_t preferred_group, uint16_t items_per_group, uint8_t*& out_bitmap, uint32_t& out_bitmap_num)
{
    uint32_t block_size = this->m_sb.get_block_size();
    uint32_t num_bgs = this->m_sb.get_bg_count();

    for (uint32_t i = 0; i < num_bgs; i++)
    {
        uint32_t current_group_idx = (preferred_group + i) % num_bgs;
        const BlockGroupDescriptor& out_group_desc = this->m_bgdt.get_bgd(current_group_idx); 

        if (out_group_desc.m_fields.bg_free_inodes_count == 0) continue;

        out_bitmap_num = out_group_desc.m_fields.bg_inode_bitmap;
        out_bitmap = new uint8_t[block_size];

        try
        {
            this->read_block(out_bitmap_num, out_bitmap, block_size);
            int free_bit_idx = utils::find_first_free_bit(out_bitmap, block_size);

            if (free_bit_idx != -1 && (uint32_t)(free_bit_idx) < items_per_group)
            {
                utils::set_bit(out_bitmap, free_bit_idx);
                return (current_group_idx * items_per_group) + free_bit_idx;
            }
            delete[] out_bitmap;
            out_bitmap = nullptr;
        }
        catch (const std::exception& e)
        {
            delete[] out_bitmap;
            out_bitmap = nullptr;
            throw;
        }
    }

    return 0;
}

uint32_t Ext2::allocate_inode(uint32_t parent_inode, uint8_t*& out_inode_bitmap, uint32_t& out_inode_bitmap_num)
{
    uint32_t preferred_group = (parent_inode > 0 && parent_inode <= this->m_sb.m_fields.s_inodes_count) ? 
                               (parent_inode - 1) /  this->m_sb.m_fields.s_inodes_per_group : 0;

    uint32_t inode_num = this->allocate(preferred_group, this->m_sb.m_fields.s_inodes_per_group, out_inode_bitmap, out_inode_bitmap_num);
    if(0 == inode_num) return 0;

    return inode_num + 1; // inodes start from 1
}

uint32_t Ext2::allocate_block(uint32_t inode_num, uint8_t*& out_block_bitmap_data, uint32_t& out_block_bitmap_num)
{
    uint32_t preferred_group = (inode_num > 0 && inode_num <= this->m_sb.m_fields.s_inodes_count && this->m_sb.m_fields.s_inodes_per_group > 0) ?
                               (inode_num - 1) / this->m_sb.m_fields.s_inodes_per_group : 0;

    uint32_t block_num = this->allocate(preferred_group, this->m_sb.m_fields.s_blocks_per_group, out_block_bitmap_data, out_block_bitmap_num);
    if(0 == block_num) return 0;

    return block_num + this->m_sb.m_fields.s_first_data_block; // blocks start from first_data_block
}

void Ext2::init_directory_block(Inode& inode, uint32_t inode_num, uint32_t parent_inode_num)
{
    uint32_t block_size = this->m_sb.get_block_size();
    uint8_t* block_buffer = new uint8_t[block_size]{};

    LinkedDirectoryEntry* dot_entry = (LinkedDirectoryEntry*)(block_buffer);
    dot_entry->inode = inode_num;
    dot_entry->name_len = 1;
    dot_entry->file_type = DirectoryEntry::FileType::EXT2_FT_DIR;
    dot_entry->name[0] = '.';
    dot_entry->rec_len = Ext2::calculate_rec_len(1);

    LinkedDirectoryEntry* dotdot_entry = (LinkedDirectoryEntry*)(block_buffer + dot_entry->rec_len);
    dotdot_entry->inode = parent_inode_num;
    dotdot_entry->name_len = 2;
    dotdot_entry->file_type = DirectoryEntry::FileType::EXT2_FT_DIR;
    dotdot_entry->name[0] = '.';
    dotdot_entry->name[1] = '.';
    dotdot_entry->rec_len = block_size - dot_entry->rec_len;

    this->write_block(inode.m_fields.i_block[0], block_buffer, block_size);

    delete[] block_buffer;
}

void Ext2::fill_entry(LinkedDirectoryEntry* entry, uint32_t inode_num, const utils::string& name, uint8_t file_type)
{
    entry->inode = inode_num;
    entry->name_len = (uint8_t)(name.size());
    entry->file_type = file_type;

    utils::memcpy((char*)(entry->name), name.c_str(), name.size());

    if (name.size() < DirectoryEntry::MAX_NAME_LEN)
    {
        entry->name[name.size()] = '\0';
    }
}

bool Ext2::try_reuse_hole(LinkedDirectoryEntry* hole, uint32_t inode_num, const utils::string& name, uint8_t file_type, uint16_t entry_len)
{
    if (hole->rec_len < entry_len) return false;

    this->fill_entry(hole, inode_num, name, file_type);

    uint16_t leftover = hole->rec_len - entry_len;
    hole->rec_len = entry_len;

    if (leftover >= Ext2::calculate_rec_len(0))
    {
        LinkedDirectoryEntry* next_hole = (LinkedDirectoryEntry*)((uint8_t*)(hole) + entry_len);

        this->fill_entry(next_hole, 0, "", DirectoryEntry::FileType::EXT2_FT_UNKNOWN);
        next_hole->rec_len = leftover;
    }
    else if (leftover > 0)
    {
        hole->rec_len += leftover;
    }

    return true;
}

bool Ext2::try_split_active_entry(LinkedDirectoryEntry* entry, uint8_t* buffer, uint32_t offset, uint32_t inode_num, const utils::string& name, uint8_t file_type, uint16_t entry_len)
{
    uint16_t current_entry_len = Ext2::calculate_rec_len(entry->name_len);

    if (entry->rec_len < current_entry_len + entry_len) return false;

    uint16_t original_rec_len = entry->rec_len;
    entry->rec_len = current_entry_len;

    LinkedDirectoryEntry* new_entry = (LinkedDirectoryEntry*)(buffer + offset + current_entry_len);
    this->fill_entry(new_entry, inode_num, name, file_type);
    new_entry->rec_len = original_rec_len - current_entry_len;

    return true;
}


bool Ext2::write_entry(uint32_t parent_block_num, uint32_t new_entry_inode_num, const utils::string& new_entry_name, uint8_t new_entry_file_type, uint16_t entry_len)
{
    uint32_t block_size = this->m_sb.get_block_size();

    uint8_t* block_buffer = new uint8_t[block_size];
    this->read_block(parent_block_num, block_buffer, block_size);

    uint32_t current_offset = 0;
    while (current_offset < block_size)
    {
        LinkedDirectoryEntry* current_disk_entry = (LinkedDirectoryEntry*)(block_buffer + current_offset);

        if (current_disk_entry->rec_len == 0) break;

        if (current_disk_entry->inode != 0 &&
            this->try_split_active_entry(current_disk_entry, block_buffer, current_offset, new_entry_inode_num, new_entry_name, new_entry_file_type, entry_len))
        {
            this->write_block(parent_block_num, block_buffer, block_size);

            delete[] block_buffer;
            return true;
        }
        else if (this->try_reuse_hole(current_disk_entry, new_entry_inode_num, new_entry_name, new_entry_file_type, entry_len))
        {
            this->write_block(parent_block_num, block_buffer, block_size);

            delete[] block_buffer;
            return true;
        }

        current_offset += current_disk_entry->rec_len;
    }


    delete[] block_buffer;
    std::cerr << "[Errir] Ext2::write_entry - no space in this block\n";
    return false;
}

void Ext2::append_dir_entry(Inode& parent_inode, uint32_t inode_num, const utils::string& entry_name, uint8_t entry_file_type)
{
    uint16_t entry_len = Ext2::calculate_rec_len((uint8_t)(entry_name.size()));

    for (int i = 0; i < 12; i++)
    {
        uint32_t parent_block_num = parent_inode.m_fields.i_block[i];
        if (parent_block_num == 0) continue; // unallocated block

        try
        {
            if (this->write_entry(parent_block_num, inode_num, entry_name, entry_file_type, entry_len))
            {
                parent_inode.set_times_now();
                return;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Error] Ext2::append_dir_entry - block: " <<  parent_block_num << " - " << e.what() << '\n';
        }
    }

    throw std::runtime_error("Ext2::append_dir_entry - No space found in parent's existing direct blocks");
}

void Ext2::write_inode(const Inode& inode_to_write, uint32_t inode_num)
{
    Inode& inode = this->m_it.get_inode(inode_num);
    Inode temp_inode = inode;
    inode = inode_to_write;

    try
    {
        inode.write(this->get_device_path());
    }
    catch(const std::exception& e)
    {
        inode = temp_inode;
        throw;
    }
}

void Ext2::write_bgd(const BlockGroupDescriptor& group_descriptor_to_write, uint32_t group_num)
{
    BlockGroupDescriptor& group_desc = this->m_bgdt.get_bgd(group_num);
    BlockGroupDescriptor temp_group_desc = group_desc;
    group_desc = group_descriptor_to_write;

    try
    {
        group_desc.write(this->get_device_path());
    }
    catch(const std::exception& e)
    {
        group_desc = temp_group_desc;
        throw;
    }
}

void Ext2::write_sb(const SuperBlock& super_block_to_write)
{
    SuperBlock temp_group_desc = this->m_sb;
    this->m_sb = super_block_to_write;

    try
    {
        this->m_sb.write(this->get_device_path());
    }
    catch(const std::exception& e)
    {
        this->m_sb = temp_group_desc ;
        throw;
    }
}

void Ext2::commit_file(
    bool is_directory, uint32_t new_block_num,
    const Inode& new_inode, uint32_t new_inode_num,
    const Inode& parent_inode, uint32_t parent_inode_num,
    uint8_t* inode_bitmap, uint32_t inode_bitmap_num,
    uint8_t* block_bitmap, uint32_t block_bitmap_num
)
{
    uint32_t new_inode_bgd_num = (new_inode_num - 1) / this->m_sb.m_fields.s_inodes_per_group;
    BlockGroupDescriptor new_inode_bgd = this->m_bgdt.get_bgd(new_inode_bgd_num);

    uint32_t new_block_bgd_num = (new_block_num - this->m_sb.m_fields.s_first_data_block) / this->m_sb.m_fields.s_blocks_per_group;
    BlockGroupDescriptor new_block_bgd = this->m_bgdt.get_bgd(new_block_bgd_num);

    if (is_directory) new_inode_bgd.m_fields.bg_used_dirs_count++;
    new_inode_bgd.m_fields.bg_free_inodes_count--;
    new_block_bgd.m_fields.bg_free_blocks_count--;

    this->write_inode(new_inode, new_inode_num);
    this->write_inode(parent_inode, parent_inode_num);

    this->write_bgd(new_inode_bgd, new_inode_bgd_num);
    if (new_inode_bgd_num != new_block_bgd_num) this->write_bgd(new_block_bgd, new_block_bgd_num);

    SuperBlock sb_copy = this->m_sb;
    sb_copy.m_fields.s_free_inodes_count--;
    sb_copy.m_fields.s_free_blocks_count--;

    this->write_sb(sb_copy);

    this->write_block(inode_bitmap_num, inode_bitmap, this->m_sb.get_block_size());
    this->write_block(block_bitmap_num, block_bitmap, this->m_sb.get_block_size());
}

void Ext2::create_file(const utils::string& path, bool is_directory)
{
    if (path.empty() || path == "/")
    {
        throw std::runtime_error("Ext2::create_file - cannot create with empty or root path.");
    }

    const utils::string entry_name = utils::get_entry_name(path);
    Ext2::check_entry_name_validity(entry_name);

    uint32_t parent_inode_num = 0;
    const utils::string parent_path = utils::get_parent_path(path);
    Inode parent_inode = this->resolve_path(parent_path, parent_inode_num); 
    if (!(parent_inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR))
    {
        throw std::runtime_error("Ext2::create_file - parent path is not a directory.");
    }

    const utils::vector<LinkedDirectoryEntry> parent_entries = this->read_dir_entries(parent_inode);
    if (0 != Ext2::get_entry_with_name(parent_entries, entry_name))
    {
        throw std::runtime_error("Ext2::create_file - entry with that name already exists.");
    }

    uint8_t* inode_bitmap = nullptr;
    uint32_t inode_bitmap_num = 0;

    uint32_t new_inode_num = this->allocate_inode(parent_inode_num, inode_bitmap, inode_bitmap_num);
    if (0 == new_inode_num)
    {
        throw std::runtime_error("Ext2::create_file - failed to allocate a new inode.");
    }

    uint8_t* block_bitmap = nullptr;
    uint32_t block_bitmap_num = 0;


    uint32_t new_block_num = this->allocate_block(new_inode_num, block_bitmap, block_bitmap_num);
    if (0 == new_block_num)
    {
        delete[] inode_bitmap;
        throw std::runtime_error("Ext2::create_file - failed to allocate a new data block.");
    }

    Inode new_inode = this->m_it.get_inode(new_inode_num);
    new_inode.init(is_directory, this->m_sb.get_block_size(), new_block_num);
    try
    {
        if (is_directory)
        {
            this->init_directory_block(new_inode, new_inode_num, parent_inode_num);
        }

        uint8_t entry_fs_type = is_directory ? DirectoryEntry::FileType::EXT2_FT_DIR : DirectoryEntry::FileType::EXT2_FT_REG_FILE;
        this->append_dir_entry(parent_inode, new_inode_num, entry_name, entry_fs_type);

        this->commit_file(
            is_directory, new_block_num,
            new_inode, new_inode_num,
            parent_inode, parent_inode_num,
            inode_bitmap, inode_bitmap_num,
            block_bitmap, block_bitmap_num
        );
    }
    catch(const std::exception& e)
    {
        delete[] inode_bitmap;
        delete[] block_bitmap;
        throw;
    }

    delete[] inode_bitmap;
    inode_bitmap= nullptr;

    delete[] block_bitmap;
    block_bitmap = nullptr;

    std::cout << (is_directory ? "Directory" : "File") << " " << path.c_str() << " created successfully.\n";
}

void Ext2::deallocate_inode_on_disk(uint32_t inode_num, bool is_directory)
{
    if (inode_num == 0 || inode_num < Inode::Reserved::EXT2_ROOT_INO)
    {
        throw std::runtime_error("Ext2::deallocate_inode_on_disk - invalid inode to deallocate");
    }

    uint32_t block_size = this->m_sb.get_block_size();

    int bit = (inode_num - 1) % this->m_sb.m_fields.s_inodes_per_group;

    BlockGroupDescriptor& bgd = this->m_bgdt.get_bgd((inode_num - 1) / this->m_sb.m_fields.s_inodes_per_group);

    uint32_t inode_bitmap_num = bgd.m_fields.bg_inode_bitmap;
    uint8_t* inode_bitmap = new uint8_t[block_size];

    try
    {
        this->read_block(inode_bitmap_num, inode_bitmap, block_size);

        if (utils::is_bit_set(inode_bitmap, bit))
        {
            utils::clear_bit(inode_bitmap, bit);
            this->write_block(inode_bitmap_num, inode_bitmap, block_size); 

            bgd.m_fields.bg_free_inodes_count++;
            if (is_directory && bgd.m_fields.bg_used_dirs_count > 0) bgd.m_fields.bg_used_dirs_count--;

            this->m_sb.m_fields.s_free_inodes_count++;
        }

        delete[] inode_bitmap;
    }
    catch (const std::exception& e)
    {
        delete[] inode_bitmap;
        throw;
    }
}

void Ext2::deallocate_block_on_disk(uint32_t block_num)
{
    if (block_num < this->m_sb.m_fields.s_first_data_block)
    {
        throw std::runtime_error("Ext2::deallocate_block_on_disk - invalid block to deallocate");
    }

    uint32_t block_size = this->m_sb.get_block_size();

    int bit = (block_num - this->m_sb.m_fields.s_first_data_block) % this->m_sb.m_fields.s_blocks_per_group;

    BlockGroupDescriptor& bgd = this->m_bgdt.get_bgd((block_num - this->m_sb.m_fields.s_first_data_block) / this->m_sb.m_fields.s_blocks_per_group);

    uint32_t block_bitmap_num = bgd.m_fields.bg_block_bitmap;
    uint8_t* block_bitmap = new uint8_t[block_size];

    try
    {
        this->read_block(block_bitmap_num, block_bitmap, block_size);
        if (utils::is_bit_set(block_bitmap, bit))
        {
            utils::clear_bit(block_bitmap, bit);
            this->write_block(block_bitmap_num, block_bitmap, block_size); 

            bgd.m_fields.bg_free_blocks_count++;
            this->m_sb.m_fields.s_free_blocks_count++;
        }

        delete[] block_bitmap;
    }
    catch (const std::exception& e)
    {
        delete[] block_bitmap;
        throw; 
    }
}

void Ext2::commit_deallocated_file(Inode& entry_inode, uint32_t entry_inode_num, Inode& parent_inode, uint32_t parent_inode_num)
{
    bool is_directory = entry_inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR;

    if (entry_inode.m_fields.i_links_count > 0)
    {
        entry_inode.m_fields.i_links_count--;
    }

    entry_inode.m_fields.i_ctime = time(nullptr);

    if (0 == entry_inode.m_fields.i_links_count)
    {
        entry_inode.m_fields.i_dtime = time(nullptr);

        for (int i = 0; i < 12; i++)
        {
            uint32_t data_block_num = entry_inode.m_fields.i_block[i];
            if (0 == data_block_num) continue;

            this->deallocate_block_on_disk(data_block_num);
            entry_inode.m_fields.i_block[i] = 0;
        }
        entry_inode.m_fields.i_blocks = 0;
        entry_inode.m_fields.i_size = 0;

        this->deallocate_inode_on_disk(entry_inode_num, is_directory);
    }

    this->write_inode(parent_inode, parent_inode_num);
    this->write_inode(entry_inode, entry_inode_num); 

    for (uint16_t i = 0; i < this->m_sb.get_bg_count(); i++)
    {
        this->write_bgd(this->m_bgdt.get_bgd(i), i);
    }

    this->m_sb.write(this->get_device_path()); 

    this->m_it.get_inode(parent_inode_num) = parent_inode;
    if (entry_inode.m_fields.i_links_count == 0) {
        Inode deallocated_marker; 
        deallocated_marker.m_fields.i_mode = 0; 
        deallocated_marker.m_fields.i_links_count = 0;
        deallocated_marker.m_fields.i_dtime = entry_inode.m_fields.i_dtime;
        this->m_it.get_inode(entry_inode_num) = deallocated_marker;
    } else {
        this->m_it.get_inode(entry_inode_num) = entry_inode;
    }

}

bool Ext2::find_and_remove_entry_from_data_block(Inode& parent_inode, uint32_t entry_data_block_num, const utils::string& entry_name)
{
    uint32_t block_size = this->m_sb.get_block_size();
    uint8_t* block_buffer = new uint8_t[block_size];

    try
    {
        this->read_block(entry_data_block_num, block_buffer, block_size);
        uint32_t current_offset = 0;
        LinkedDirectoryEntry* previous_entry_ptr = nullptr;

        while (current_offset < block_size)
        {
            LinkedDirectoryEntry* current_entry_ptr = (LinkedDirectoryEntry*)(block_buffer + current_offset);

            if (current_entry_ptr->inode != 0 && entry_name.size() == (size_t)(current_entry_ptr->name_len) &&
                0 == utils::strncmp(entry_name.c_str(), (const char*)(current_entry_ptr->name), current_entry_ptr->name_len))
            {
                current_entry_ptr->inode = 0;
                if (previous_entry_ptr != nullptr && previous_entry_ptr->inode == 0)
                {
                    previous_entry_ptr->rec_len += current_entry_ptr->rec_len;
                }

                this->write_block(entry_data_block_num, block_buffer, block_size);
                parent_inode.set_times_now();

                return true;
            }

            previous_entry_ptr = current_entry_ptr;
            current_offset += current_entry_ptr->rec_len;
        }
    }
    catch (const std::exception& e)
    {
        delete[] block_buffer;
        throw;
    }

    delete[] block_buffer;
    return false;
}

uint32_t Ext2::get_entry_data_block_and_number(const Inode& parent_inode, const utils::string& entry_name, uint32_t& entry_inode_number)
{
    for(int i = 0; i < 12; i++)
    {
        uint32_t data_block = parent_inode.m_fields.i_block[i];
        if (data_block == 0) continue;

        utils::vector<LinkedDirectoryEntry> current_block_entries;
        this->read_directory_block(data_block, current_block_entries);

        for(size_t j = 0; j < current_block_entries.size(); j++)
        {
            const LinkedDirectoryEntry& entry = current_block_entries[j];

            if (entry.inode != 0 && entry_name.size() == (size_t)(entry.name_len) && 0 == utils::strncmp(entry_name.c_str(), (const char*)(entry.name), entry.name_len))
            {
                entry_inode_number = entry.inode;
                return data_block;
            }
        }
    }

    return 0;
}

bool Ext2::has_entry_children(const Inode& entry_inode)
{
    utils::vector<LinkedDirectoryEntry> target_children = this->read_dir_entries(entry_inode);
    for (size_t i = 0; i < target_children.size(); i++)
    {
        const LinkedDirectoryEntry& child = target_children[i];

        if (child.inode == 0) continue;

        char temp_name[DirectoryEntry::MAX_NAME_LEN]{};
        uint8_t len = std::min(child.name_len, (uint8_t)(DirectoryEntry::MAX_NAME_LEN - 1));

        utils::memcpy(temp_name, (const char*)(child.name), len);
        temp_name[len] = '\0';

        if (0 != utils::strcmp(temp_name, ".") && 0 != utils::strcmp(temp_name, ".."))
        {
            return true;
        }
    }

    return false;
}

void Ext2::remove_entry_children(const utils::string& path, const Inode& entry_inode)
{
    utils::vector<LinkedDirectoryEntry> children_to_process = this->read_dir_entries(entry_inode);

    for (size_t i = 0; i < children_to_process.size(); i++)
    {
        const LinkedDirectoryEntry& child = children_to_process[i];
        if (child.inode == 0) continue;

        char temp_name[DirectoryEntry::MAX_NAME_LEN]{};
        uint8_t len = std::min(child.name_len, (uint8_t)(DirectoryEntry::MAX_NAME_LEN - 1));

        utils::memcpy(temp_name, (const char*)(child.name), len);
        temp_name[len] = '\0';

        if (utils::strcmp(temp_name, ".") != 0 && utils::strcmp(temp_name, "..") != 0)
        {
            utils::string child_full_path = path;
            if (child_full_path.back() != '/') child_full_path += "/";
            child_full_path += temp_name;

            this->remove_file(child_full_path, true);
        }
    }

}

void Ext2::remove_file(const utils::string& path, bool recursive)
{
    if (path.empty() || path == "/")
    {
        throw std::runtime_error("Ext2::remove_file - cannot remove with empty or root path.");
    }

    const utils::string entry_name = utils::get_entry_name(path);
    if(entry_name == ".." || "." == entry_name)
    {
        throw std::runtime_error("Ext2::remove_file - cannot remove current or previous dir");
    }

    uint32_t parent_inode_num = 0;
    const utils::string parent_path = utils::get_parent_path(path);
    Inode parent_inode = this->resolve_path(parent_path, parent_inode_num); 

    uint32_t entry_inode_num = 0;
    uint32_t entry_data_block_num = this->get_entry_data_block_and_number(parent_inode, entry_name, entry_inode_num);
    if (0 == entry_data_block_num)
    {
        throw std::runtime_error("Ext2::remove_file - couldn't find entry data block");
    }

    Inode entry_inode = this->m_it.get_inode(entry_inode_num);
    bool is_directory = entry_inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFDIR;
    if(is_directory && this->has_entry_children(entry_inode))
    {
        if (!recursive)
        {
            throw std::runtime_error("Ext2::remove_file - directory not empty and recursive not set.");
        }

        this->remove_entry_children(path, entry_inode);
    }

    if (!this->find_and_remove_entry_from_data_block(parent_inode, entry_data_block_num, entry_name))
    {
        throw std::runtime_error("Ext2::remove_file - failed to modify parent directory block.");
    }

    this->commit_deallocated_file(entry_inode, entry_inode_num, parent_inode, parent_inode_num);

    std::cout << (is_directory ? "Directory " : "File ") << path.c_str() << " removed successfully.\n";
}

void Ext2::process_block(utils::vector<uint8_t>& file_data, uint32_t file_size, uint32_t& bytes_read, uint32_t block_num, uint8_t* block_buffer)
{
    uint32_t block_size = this->m_sb.get_block_size();

    if (block_num == 0 || bytes_read >= file_size) return;

    this->read_block(block_num, block_buffer, block_size);

    uint32_t remaining_bytes_in_file = file_size - bytes_read;
    uint32_t bytes_to_copy_from_block = (remaining_bytes_in_file < block_size) ? remaining_bytes_in_file : block_size;

    for (uint32_t i = 0; i < bytes_to_copy_from_block; i++)
    {
        file_data.push_back(block_buffer[i]);
    }
    bytes_read += bytes_to_copy_from_block;
}

void Ext2::process_single_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer)
{
    utils::vector<uint32_t> direct_blocks = this->read_indirect_block_pointers(block_num);
    for (size_t i = 0; i < direct_blocks.size() && bytes_read < file_size; i++)
    {
        this->process_block(file_data, file_size, bytes_read, direct_blocks[i], block_buffer);
    }
}

void Ext2::process_double_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer)
{
    utils::vector<uint32_t> single_indirect_blocks = this->read_indirect_block_pointers(block_num);
    for (size_t i = 0; i < single_indirect_blocks.size() && bytes_read < file_size; i++)
    {
        this->process_single_indirect_blocks(file_data, bytes_read, file_size, single_indirect_blocks[i], block_buffer);
    }
}

void Ext2::process_triple_indirect_blocks(utils::vector<uint8_t>& file_data, uint32_t& bytes_read, uint32_t file_size, uint32_t block_num, uint8_t* block_buffer)
{
    utils::vector<uint32_t> double_indirect_blocks = this->read_indirect_block_pointers(block_num);
    for (size_t i = 0; i < double_indirect_blocks.size() && bytes_read < file_size; i++)
    {
        this->process_double_indirect_blocks(file_data, bytes_read, file_size, double_indirect_blocks[i], block_buffer);
    }
}

utils::vector<uint8_t> Ext2::read_file_data(const Inode& inode)
{
    uint32_t file_size = inode.m_fields.i_size;
    utils::vector<uint8_t> file_data(file_size);

    uint32_t block_size = this->m_sb.get_block_size();
    uint8_t* block_buffer = new uint8_t[block_size];
    uint32_t bytes_read = 0;

    try
    {
        for (int i = 0; i < 12 && bytes_read < file_size; i++)
        {
            this->process_block(file_data, file_size, bytes_read, inode.m_fields.i_block[i], block_buffer);
        }

        if (bytes_read < file_size && inode.m_fields.i_block[12] != 0)
        {
            this->process_single_indirect_blocks(file_data, bytes_read, file_size, inode.m_fields.i_block[12], block_buffer);
        }

        if (bytes_read < file_size && inode.m_fields.i_block[13] != 0)
        {
            this->process_double_indirect_blocks(file_data, bytes_read, file_size, inode.m_fields.i_block[13], block_buffer);
        }

        if (bytes_read < file_size && inode.m_fields.i_block[14] != 0)
        {
            this->process_triple_indirect_blocks(file_data, bytes_read, file_size, inode.m_fields.i_block[14], block_buffer);
        }
    }
    catch(const std::exception& e)
    {
        delete[] block_buffer;
        throw;
    }

    delete[] block_buffer;
    return file_data;
}

utils::vector<uint8_t> Ext2::read_file(const utils::string& path)
{
    uint32_t inode_num = 0;
    Inode& inode = this->resolve_path(path, inode_num);

    if (!(inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFREG))
    {
        throw std::runtime_error("Ext2::read_file - Path does not point to a regular file");
    }

    utils::vector<uint8_t> file_data = this->read_file_data(inode);

    inode.m_fields.i_atime = time(nullptr);
    this->write_inode(inode, inode_num);

    return file_data;
}

void Ext2::allocate_and_fill_data_block(uint32_t inode_num, uint32_t& target_block_storage_location, const utils::vector<uint8_t>& data, uint32_t& data_idx, uint32_t& total_blocks_used_by_inode, uint8_t* temp_data_chunk_buffer, uint8_t*& block_bitmap_buffer, uint32_t& current_block_bitmap_num)
{
    uint32_t block_size = this->m_sb.get_block_size();
    uint32_t new_block_num = this->allocate_block(inode_num, block_bitmap_buffer, current_block_bitmap_num);
    if (0 == new_block_num)
    {
        throw std::runtime_error("Ext2::allocate_and_fill_data_block - Failed to allocate data block.");
    }
    target_block_storage_location = new_block_num;

    for (uint32_t i = 0; i < block_size; i++)
    {
        if (data_idx < data.size())
        {
            temp_data_chunk_buffer[i] = data[data_idx++];
        }
        else
        {
            temp_data_chunk_buffer[i] = 0;
        }
    }

    this->write_block(new_block_num, temp_data_chunk_buffer, block_size);
    total_blocks_used_by_inode++;

    if (block_bitmap_buffer)
    {
        this->write_block(current_block_bitmap_num, block_bitmap_buffer, block_size);
        delete[] block_bitmap_buffer;
        block_bitmap_buffer = nullptr;
    }
}

void Ext2::process_direct_block_writes(Inode& inode, uint32_t inode_num, const utils::vector<uint8_t>& data, uint32_t& data_idx, uint32_t& total_blocks_used_by_inode, uint8_t* temp_data_chunk_buffer, uint8_t*& block_bitmap_buffer, uint32_t& current_block_bitmap_num)
{
    for (int i = 0; i < 12; i++)
    {
        if (data_idx >= data.size()) break;

        this->allocate_and_fill_data_block(
            inode_num, 
            inode.m_fields.i_block[i],
            data, 
            data_idx, 
            total_blocks_used_by_inode,
            temp_data_chunk_buffer, 
            block_bitmap_buffer, 
            current_block_bitmap_num
        );
    }
}

bool Ext2::process_indirect_block_writes_recursive(uint32_t inode_num, int level, uint32_t* indirect_block_id_storage_location, const utils::vector<uint8_t>& data, uint32_t& data_idx, uint32_t& total_blocks_used_by_inode, uint8_t* temp_data_chunk_buffer, uint8_t*& block_bitmap_buffer, uint32_t& current_block_bitmap_num)
{
    uint32_t fs_block_size = this->m_sb.get_block_size();
    uint32_t current_indirect_block_bn = *indirect_block_id_storage_location;

    if (current_indirect_block_bn == 0)
    {
        current_indirect_block_bn = this->allocate_block(inode_num, block_bitmap_buffer, current_block_bitmap_num);
        if (current_indirect_block_bn == 0)
        {
            throw std::runtime_error("Ext2::process_indirect_block_writes_recursive - Failed to allocate an indirect block.");
        }


        *indirect_block_id_storage_location = current_indirect_block_bn;
        total_blocks_used_by_inode++;
        if (block_bitmap_buffer)
        {
            this->write_block(current_block_bitmap_num, block_bitmap_buffer, fs_block_size);
            delete[] block_bitmap_buffer;
            block_bitmap_buffer = nullptr;
        }
    }

    uint32_t num_pointers_per_block = fs_block_size / sizeof(uint32_t);
    uint32_t* pointers_for_current_indirect_block = new uint32_t[num_pointers_per_block]{};

    bool data_processed_at_this_level = false;
    for (uint32_t i = 0; i < num_pointers_per_block; i++)
    {
        if (data_idx >= data.size()) break;

        if (level > 0)
        {
            if(this->process_indirect_block_writes_recursive(
                inode_num, 
                level - 1, 
                &pointers_for_current_indirect_block[i], 
                data, 
                data_idx, 
                total_blocks_used_by_inode, 
                temp_data_chunk_buffer, 
                block_bitmap_buffer, 
                current_block_bitmap_num
            ))
            {
                data_processed_at_this_level = true;
            }
        }
        else
        {
            this->allocate_and_fill_data_block(
                inode_num, 
                pointers_for_current_indirect_block[i], 
                data, 
                data_idx, 
                total_blocks_used_by_inode, 
                temp_data_chunk_buffer, 
                block_bitmap_buffer, 
                current_block_bitmap_num
            );
            data_processed_at_this_level = true;
        }
    }

    // write if it's newly allocated or if data was processed into it
    if (data_processed_at_this_level || *indirect_block_id_storage_location == current_indirect_block_bn)
    {
         this->write_block(current_indirect_block_bn, (uint8_t*)pointers_for_current_indirect_block, fs_block_size);
    }
    delete[] pointers_for_current_indirect_block;

    return data_processed_at_this_level;
}

void Ext2::perform_inode_data_write(Inode& inode, uint32_t inode_num, const utils::vector<uint8_t>& data_to_write)
{
    uint32_t data_idx = 0;
    uint32_t total_blocks_used_by_inode = 0;
    uint32_t block_size = this->m_sb.get_block_size();

    uint8_t* temp_data_chunk_buffer = new uint8_t[block_size];
    uint8_t* block_bitmap_buffer = nullptr; 
    uint32_t current_block_bitmap_num = 0;  

    try
    {
        this->process_direct_block_writes(inode, inode_num, data_to_write, data_idx, total_blocks_used_by_inode, temp_data_chunk_buffer, block_bitmap_buffer, current_block_bitmap_num);

        if (data_idx < data_to_write.size())
        {
            this->process_indirect_block_writes_recursive(inode_num, 0, &inode.m_fields.i_block[12], data_to_write, data_idx, total_blocks_used_by_inode, temp_data_chunk_buffer, block_bitmap_buffer, current_block_bitmap_num);
        }

        if (data_idx < data_to_write.size())
        {
            this->process_indirect_block_writes_recursive(inode_num, 1, &inode.m_fields.i_block[13], data_to_write, data_idx, total_blocks_used_by_inode, temp_data_chunk_buffer, block_bitmap_buffer, current_block_bitmap_num);
        }

        if (data_idx < data_to_write.size())
        {
            this->process_indirect_block_writes_recursive(inode_num, 2, &inode.m_fields.i_block[14], data_to_write, data_idx, total_blocks_used_by_inode, temp_data_chunk_buffer, block_bitmap_buffer, current_block_bitmap_num);
        }

        inode.m_fields.i_size = data_to_write.size();
        inode.m_fields.i_blocks = total_blocks_used_by_inode * (block_size / 512);
    }
    catch (const std::runtime_error& e)
    {
        delete[] temp_data_chunk_buffer;
        delete[] block_bitmap_buffer;
        throw; 
    }

    delete[] temp_data_chunk_buffer;
    delete[] block_bitmap_buffer;
}

void Ext2::deallocate_direct_blocks(Inode& inode)
{
    for (int i = 0; i < 12; i++)
    {
        if (inode.m_fields.i_block[i] != 0)
        {
            this->deallocate_block_on_disk(inode.m_fields.i_block[i]);
            inode.m_fields.i_block[i] = 0;
        }
    }
}

void Ext2::deallocate_indirect_blocks_recursive(uint32_t block_num, int level)
{
    if (block_num == 0) return;

    utils::vector<uint32_t> pointers = this->read_indirect_block_pointers(block_num);

    for (size_t i = 0; i < pointers.size(); i++)
    {
        if (pointers[i] == 0) continue;

        if (level > 0)
        {
            this->deallocate_indirect_blocks_recursive(pointers[i], level - 1);
        }
        else
        {
            this->deallocate_block_on_disk(pointers[i]);
        }
    }

    this->deallocate_block_on_disk(block_num);
}

void Ext2::deallocate_inode_content(Inode& inode)
{
    this->deallocate_direct_blocks(inode);

    if (inode.m_fields.i_block[12] != 0)
    {
        this->deallocate_indirect_blocks_recursive(inode.m_fields.i_block[12], 0);
        inode.m_fields.i_block[12] = 0;
    }
    if (inode.m_fields.i_block[13] != 0)
    {
        this->deallocate_indirect_blocks_recursive(inode.m_fields.i_block[13], 1);
        inode.m_fields.i_block[13] = 0;
    }
    if (inode.m_fields.i_block[14] != 0)
    {
        this->deallocate_indirect_blocks_recursive(inode.m_fields.i_block[14], 2);
        inode.m_fields.i_block[14] = 0;
    }

    inode.m_fields.i_size = 0;
    inode.m_fields.i_blocks = 0;
}

void Ext2::write_file(const utils::string& path, const utils::vector<uint8_t>& data_to_write_param, bool append)
{
    uint32_t inode_num = 0;
    Inode& inode = this->resolve_path(path, inode_num);

    if (!(inode.m_fields.i_mode & Inode::Mode::EXT2_S_IFREG))
    {
        throw std::runtime_error("Ext2::write_file - Path does not point to a regular file");
    }

    utils::vector<uint8_t> final_data_to_write;
    if (append)
    {
        final_data_to_write = this->read_file_data(inode);
        for(size_t i = 0; i < data_to_write_param.size(); i++)
        {
            final_data_to_write.push_back(data_to_write_param[i]);
        }
    }
    else
    {
        final_data_to_write = data_to_write_param;
    }

    this->deallocate_inode_content(inode);

    this->perform_inode_data_write(inode, inode_num, final_data_to_write);

    inode.m_fields.i_mtime = time(nullptr);
    inode.m_fields.i_ctime = time(nullptr);
    this->write_inode(inode, inode_num);

    this->m_sb.write(this->get_device_path());
    for (uint16_t i = 0; i < this->m_sb.get_bg_count(); i++)
    {
        this->m_bgdt.write_bgd(this->get_device_path(), i); 
    }
}

