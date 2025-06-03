#pragma once

#include <optional>
#include <iostream>

namespace utils
{

class string
{
public:
    static constexpr const size_t DEFAULT_CAPACITY = 16;
// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    string();
    string(size_t capacity);
    string(const char* s, size_t count);
    string(const char* s);
    string(const string& other);

    ~string();

    string(string&& other) noexcept;

    string& operator=(string&& other) noexcept;

    string& operator=(const string& other);

    string& operator=(const char* s);

// ---------------- PUBLIC OPERATORS ----------------
    char& operator[](size_t index);

    const char& operator[](size_t index) const;

    string& operator+=(const string& other);

    string& operator+=(const char* s);

// ---------------- PUBLIC METHODS ----------------
    size_t size() const;

    bool empty() const;

    const char* c_str() const;

    void clear();

    std::optional<size_t> find(char ch, size_t pos = 0) const;

    string substr(size_t pos = 0, size_t count = (size_t)-1) const;

    std::optional<size_t> find_last_of(char ch) const;

    char back() const;

private:
// ---------------- PRIVATE VARIABLES ----------------
    char* m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

// ---------------- PRIVATE METHODS ----------------
    void realloc(size_t capacity);
    void free();
    void copy_from(const string& other);
    void copy_from(const char* s);
    void move_from(string&& other);
};

string operator+(const utils::string& lhs, const utils::string& rhs);
string operator+(const utils::string& lhs, const char* rhs);
string operator+(const char* lhs, const utils::string& rhs);

bool operator==(const utils::string& lhs, const utils::string& rhs); 
bool operator==(const utils::string& lhs, const char* rhs);
bool operator==(const char* lhs, const utils::string& rhs);

bool operator!=(const utils::string& lhs, const utils::string& rhs);
bool operator!=(const utils::string& lhs, const char* rhs);
bool operator!=(const char* lhs, const utils::string& rhs);

std::ostream& operator<<(std::ostream& os, const utils::string& str);

}
