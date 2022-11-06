#include "utils.hpp"

File::File(const char* filepath)
{
    m_handle = open_or_panic(filepath);
    m_size = get_file_size(m_handle);

    // TODO: Remove malloc when memory strategy finalized.
    m_data = malloc(m_size+1);
    memset(m_data, 0, m_size+1);
    fread(m_data, m_size, 1, static_cast<FILE*>(m_handle));
}

size_t File::get_file_size(void* handle)
{
    FILE* file_handle = static_cast<FILE*>(handle);
    assert(file_handle);

    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    rewind(file_handle);

    return file_size;
}

FILE* File::open_or_panic(const char* filepath)
{
    FILE* file = fopen(filepath, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    return file;
}

File::~File()
{
    if (m_handle)
    {
        FILE* file_handle = static_cast<FILE*>(m_handle);
        fclose(file_handle);
    }
}

void* File::get_data()
{
    return m_data;    
}
