#pragma once

class File
{
    void* m_handle;
    void* m_data;
    size_t m_size;

public:

    File(const char* filepath);
    ~File();
    void* get_data();

private:
    size_t get_file_size(void* m_handle);
    FILE* open_or_panic(const char* filepath);
};

template<typename T>
static inline T min(T a, T b)
{
    return a < b ? a : b;
}

template<typename T>
static inline T max(T a, T b)
{
    return a > b ? a : b;
}
