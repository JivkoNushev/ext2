#pragma once

#include "cstring.h"
#include <cstddef>
#include <cstdint>
#include <utility>

namespace utils
{

template <class T>
class vector
{
public:
// ---------------- PUBLIC CONSTANTS ----------------
    static constexpr const uint32_t DEFAULT_CAPACITY = 16;

// ---------------- CONSTRUCTORS/DESTRUCTORS ----------------
    vector();

    vector(size_t capacity);

    vector(const T* data, size_t size);

    ~vector();

    vector(const vector<T>& other);

    vector(vector<T>&& other) noexcept;

    vector& operator=(const vector<T>& other);

    vector& operator=(vector<T>&& other) noexcept;

// ---------------- PUBLIC OPERATORS ----------------
    T& operator[](size_t index);

    const T& operator[](size_t index) const;

// ---------------- PUBLIC METHODS ----------------
    void push_back(const T& value);

    void push_back(T&& value);

    void clear() noexcept;

    const T* raw() const;

    size_t size() const noexcept;

    bool empty() const noexcept;

private:
// ---------------- PRIVATE VARIABLES ----------------
    T* m_data;
    size_t m_size;
    size_t m_capacity;

// ---------------- PRIVATE METHODS ----------------
    void realloc();

    void free();
    void copy_from(const vector<T>& other);
    void move_from(vector<T>&& other) noexcept;
};

}

template<class T>
utils::vector<T>::vector() :
    m_size(0),
    m_capacity(vector::DEFAULT_CAPACITY)
{
    this->m_data = new T[this->m_capacity];
}

template<class T>
utils::vector<T>::vector(size_t capacity) :
    m_size(0),
    m_capacity(capacity)
{
    this->m_data = new T[this->m_capacity];
}

template<class T>
utils::vector<T>::vector(const T* data, size_t size) :
    m_size(size),
    m_capacity(size)
{
    this->m_data = new T[this->m_capacity];
    for(size_t i = 0; i < size; i++)
    {
        this->m_data[i] = data[i];
    }
}

template<class T>
utils::vector<T>::~vector()
{
    delete[] this->m_data;
    this->m_data = nullptr;
}

template<class T>
utils::vector<T>::vector(const vector<T>& other)
{
    this->copy_from(other);
}

template<class T>
utils::vector<T>::vector(vector<T>&& other) noexcept
{
    this->move_from(std::move(other));
}

template<class T>
utils::vector<T>& utils::vector<T>::operator=(const vector<T>& other)
{
    if(this != &other)
    {
        this->free();
        this->copy_from(other);
    }

    return *this;
}

template<class T>
utils::vector<T>& utils::vector<T>::operator=(vector<T>&& other) noexcept
{
    if(this != &other)
    {
        this->free();
        this->move_from(std::move(other));
    }

    return *this;
}

template<class T>
T& utils::vector<T>::operator[](size_t index)
{
    return m_data[index];
}

template<class T>
const T& utils::vector<T>::operator[](size_t index) const
{
    return m_data[index];
}

template<class T>
void utils::vector<T>::push_back(const T& value)
{
    if (this->m_size >= this->m_capacity)
    {
        this->realloc();
    }

    this->m_data[this->m_size++] = value;
}

template<class T>
void utils::vector<T>::push_back(T&& value)
{
    if (this->m_size >= this->m_capacity)
    {
        this->realloc();
    }

    this->m_data[this->m_size++] = std::move(value);
}

template<class T>
void utils::vector<T>::clear() noexcept
{
    for (size_t i = 0; i < this->m_size; i++)
    {
        this->m_data[i].~T();
    }

    this->m_size = 0;
}

template<class T>
const T* utils::vector<T>::raw() const
{
    return this->m_data;
}

template<class T>
size_t utils::vector<T>::size() const noexcept
{
    return this->m_size;
}

template<class T>
bool utils::vector<T>::empty() const noexcept
{
    return 0 == this->m_size;
}

template<class T>
void utils::vector<T>::realloc()
{
    this->m_capacity *= 2;
    T* new_data = new T[this->m_capacity];

    for(size_t i = 0; i < this->m_size; i++)
    {
        new_data[i] = this->m_data[i];
    }

    delete[] this->m_data;
    this->m_data = new_data;
}

template<class T>
void utils::vector<T>::free()
{
    delete[] this->m_data;
    this->m_data = nullptr;
}

template<class T>
void utils::vector<T>::copy_from(const vector<T>& other)
{
    this->m_size = other.m_size;
    this->m_capacity = other.m_capacity;
    this->m_data = new T[this->m_capacity];
    for(size_t i = 0; i < this->m_size; i++)
    {
        this->m_data[i] = other.m_data[i];
    }
}

template<class T>
void utils::vector<T>::move_from(vector<T>&& other) noexcept
{
    this->m_size = other.m_size;
    this->m_capacity = other.m_capacity;
    this->m_data = other.m_data;

    other.m_size = 0;
    other.m_capacity = 0;
    other.m_data = nullptr;
}

