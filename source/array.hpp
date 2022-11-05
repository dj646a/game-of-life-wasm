#pragma once

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include "utils.hpp"

template<typename T>
class Array
{
    T* m_buffer;
    size_t m_used;
    size_t m_size;
    
public:
    Array()
    : m_size(0)
    , m_used(0)
    , m_buffer(nullptr)
    {}

    Array(size_t size)
    : m_size(size)
    , m_used(0)
    {
        size_t num_of_bytes = sizeof(T) * size;
        m_buffer = static_cast<T*>(malloc(num_of_bytes));
        memset(m_buffer, 0, num_of_bytes);
    }

    ~Array()
    {
        free(m_buffer);
        memset(this, 0, sizeof(*this));
    }

    void push(T value)
    {
        assert(m_used != m_size);
        m_buffer[m_used++] = value;
    }

    T pop()
    {
        assert(m_used != 0);
        T copy_value = m_buffer[--m_used];
        return copy_value;
    }

    T& first()
    {
        assert(m_size > 0 && m_used > 0);
        return m_buffer[0];
    }

    T& last()
    {
        assert(m_size > 0 && m_used > 0);
        return m_buffer[m_used-1];
    }

    // TODO: Overload [] operator
    T& get(size_t index)
    {
        assert(index >= 0 && index < m_size);
        return m_buffer[index];
    }

    // TODO: Overload [] operator to get a reference to the array index
    void set(size_t index, T value)
    {
        assert(index >= 0 && index < m_size);
        
        m_used = max(index, m_used);
        m_buffer[index] = value;
    }

    void resize(size_t new_size)
    {
        if (m_buffer)
            free(m_buffer);

        m_size = new_size;
        m_used = 0;

        size_t num_of_bytes = sizeof(T) * new_size;
        m_buffer = static_cast<T*>(malloc(num_of_bytes));
        memset(m_buffer, 0, new_size);
    }

    T* get_underlying_buffer()
    {
        return m_buffer;
    }

    size_t get_size()
    {
        return m_size;
    }
};
