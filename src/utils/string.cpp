#include <stdexcept>

#include "string.h"
#include "cstring.h"

utils::string::string() :
    m_size(0),
    m_capacity(string::DEFAULT_CAPACITY)
{
    this->m_data = new char[this->m_capacity]{};
}

utils::string::string(size_t capacity) :
    m_size(0),
    m_capacity(capacity)
{
    this->m_data = new char[this->m_capacity]{};
}

utils::string::string(const char* s, size_t count)
{
    if(!s || 0 == count)
    {
        this->m_size = 0;
        this->m_capacity = string::DEFAULT_CAPACITY;
        this->m_data = new char[this->m_capacity]{};
        return;
    }

    this->m_size = count;
    this->m_capacity = this->m_size + 1;
    this->m_data = new char[this->m_capacity];
    utils::memcpy(this->m_data, s, this->m_size);
    this->m_data[this->m_size] = '\0';
}

utils::string::~string()
{
    delete[] this->m_data;
    this->m_data = nullptr;
}

utils::string::string(const char* s)
{
    this->copy_from(s);
}

utils::string::string(const utils::string& other)
{
    this->copy_from(other);
}

utils::string::string(string&& other) noexcept
{
    this->move_from(std::move(other));
}

utils::string& utils::string::operator=(const utils::string& other)
{
    if(this != &other)
    {
        this->free();
        this->copy_from(other);
    }

    return *this;
}

utils::string& utils::string::operator=(string&& other) noexcept
{
    if(this != &other)
    {
        this->free();
        this->move_from(std::move(other));
    }

    return *this;
}

utils::string& utils::string::operator=(const char* s)
{
    if(0 != utils::strcmp(this->m_data, s))
    {
        this->free();
        this->copy_from(s);
    }

    return *this;
}

char& utils::string::operator[](size_t index)
{
    if(index >= this->m_size) throw std::out_of_range("utils::string - Invalid index");

    return this->m_data[index];
}

const char& utils::string::operator[](size_t index) const
{
    if(index >= this->m_size) throw std::out_of_range("utils::string - Invalid index");

    return this->m_data[index];
}

utils::string& utils::string::operator+=(const utils::string& other)
{
    if (other.m_size == 0) return *this;

    size_t new_length = this->m_size + other.m_size;
    if (new_length + 1 > this->m_capacity)
    {
        this->realloc(new_length);
    }

    utils::strcat(this->m_data, other.m_data);
    this->m_size = new_length;

    return *this;
}

utils::string& utils::string::operator+=(const char* s)
{
    if (!s || s[0] == '\0') return *this;

    size_t s_len = utils::strlen(s);
    size_t new_length = this->m_size + s_len;
    if (new_length + 1 > this->m_capacity)
    {
        this->realloc(new_length);
    }

    utils::strcat(this->m_data, s);
    this->m_size = new_length;

    return *this;
}

size_t utils::string::size() const
{
    return this->m_size;
}

bool utils::string::empty() const
{
    return this->m_size == 0;
}

const char* utils::string::c_str() const
{
    return this->m_data ? this->m_data : "";
}

void utils::string::clear()
{
    this->m_size = 0;
    if (this->m_data) this->m_data[0] = '\0';
}

std::optional<size_t> utils::string::find(char ch, size_t pos) const
{
    if (pos >= this->m_size) return std::nullopt;

    const char* found = utils::strchr(this->m_data + pos, ch);
    return found ? (std::optional<size_t>)(found - this->m_data) : std::nullopt;
}

utils::string utils::string::substr(size_t pos, size_t count) const
{
    if (pos > this->m_size)
    {
        throw std::out_of_range("string::substr - pos out of range");
    }

    if (count > this->m_size - pos)
    {
        count = this->m_size - pos;
    }

    return string(this->m_data + pos, count);
}

std::optional<size_t> utils::string::find_last_of(char ch) const
{
    if (!this->m_data || this->m_size == 0) return std::nullopt;

    for (size_t i = this->m_size; i-- > 0;) // size_t so we can't do i >= 0; i--
    {
        if (this->m_data[i] == ch) return i;
    }

    return std::nullopt;
}

char utils::string::back() const
{
    if (!this->m_data || this->m_size == 0) return '\0';

    return this->m_data[this->m_size - 1];
}

void utils::string::realloc(size_t capacity)
{
    size_t new_capacity = capacity + 1;
    if (new_capacity <= this->m_capacity && this->m_data != nullptr) return;

    char* new_data= new char[new_capacity];

    if (this->m_data && this->m_size > 0)
    {
        utils::memcpy(new_data, this->m_data, this->m_size);
    }
    new_data[this->m_size] = '\0'; 

    delete[] this->m_data;
    this->m_data = new_data;
    this->m_capacity = new_capacity;
}

void utils::string::free()
{
    delete[] this->m_data;
    this->m_data = nullptr;
}

void utils::string::copy_from(const utils::string& other)
{
    this->m_size = other.m_size;
    this->m_capacity = other.m_capacity;
    this->m_data = new char[this->m_capacity];
    utils::strcpy(this->m_data, other.m_data);
    this->m_data[this->m_size] = '\0';
}

void utils::string::copy_from(const char* s)
{
    this->m_size = utils::strlen(s);
    this->m_capacity = this->m_size + 1;
    this->m_data = new char[this->m_capacity];
    utils::strcpy(this->m_data, s);
    this->m_data[this->m_size] = '\0';
}

void utils::string::move_from(string&& other)
{
    this->m_size = other.m_size;
    this->m_capacity = other.m_capacity;
    this->m_data = other.m_data;

    other.m_size = 0;
    other.m_capacity = 0;
    other.m_data = nullptr;
}

utils::string utils::operator+(const utils::string& lhs, const utils::string& rhs)
{
    utils::string result = lhs;
    result += rhs;
    return result;
}

utils::string utils::operator+(const utils::string& lhs, const char* rhs)
{
    utils::string result = lhs;
    result += rhs;
    return result;
}

utils::string utils::operator+(const char* lhs, const utils::string& rhs)
{
    utils::string result = lhs;
    result += rhs;
    return result;
}

bool utils::operator==(const utils::string& lhs, const utils::string& rhs)
{
    return (lhs.size() == rhs.size()) && (utils::strcmp(lhs.c_str(), rhs.c_str()) == 0);
}

bool utils::operator==(const utils::string& lhs, const char* rhs)
{
    return utils::strcmp(lhs.c_str(), rhs ? rhs : "") == 0;
}

bool utils::operator==(const char* lhs, const utils::string& rhs)
{
    return utils::strcmp(lhs ? lhs : "", rhs.c_str()) == 0;
}

bool utils::operator!=(const utils::string& lhs, const utils::string& rhs)
{
    return !(lhs == rhs);
}

bool utils::operator!=(const utils::string& lhs, const char* rhs)
{
    return !(lhs == rhs);
}

bool utils::operator!=(const char* lhs, const utils::string& rhs)
{
    return !(lhs == rhs);
}

std::ostream& utils::operator<<(std::ostream& os, const utils::string& str)
{
    os << str.c_str();
    return os;
}
