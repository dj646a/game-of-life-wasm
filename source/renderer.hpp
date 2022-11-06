#pragma once

#include "array.hpp"

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef signed   int GLint;

struct Color { float r, g, b, a; };

static const Color COLOR_RED   {1, 0, 0, 1};
static const Color COLO_GREEN  {0, 1, 0, 1};
static const Color COLOR_BLUE  {0, 0, 1, 1};
static const Color COLOR_WHITE {1, 1, 1, 1};
static const Color COLOR_BLACK {0, 0, 0, 1};

template<typename T>
union Vec2
{
    struct { T x, y; };
    struct { T w, h; };
    struct { T s, t; };
};

template<typename T>
union Vec4
{
    struct { T x0, y0, x1, y1; };
    struct { T s0, t0, s1, t1; };
};

template <typename T>
class Bitmap
{
    T* m_pixels;
    struct { int w, h; } m_size;
    int m_channels;
    int m_stride;

public:
    Bitmap(int width, int height, int channels)
    : m_size({ width, height })
    , m_channels(channels)
    , m_stride(width * channels)
    {
        // TODO: Remove malloc when memory strategy finalized.
        m_pixels = (T*) malloc(width * height * channels);
        memset(m_pixels, 0, width * height * channels);
    }

    Bitmap()
    {
        free(m_pixels);
        memset(this, 0, sizeof(*this));
    }

    T* get_pixel_buffer()
    {
        return m_pixels;
    }

    int get_width()
    {
        return m_size.w;
    }

    int get_height()
    {
        return m_size.h;
    }

    int get_stride()
    {
        return m_stride;
    }

    int get_channels()
    {
        return m_channels;
    }

    void copy_grayscale_as_rgba(Bitmap<uint8_t>& grayscale_bitmap)
    {
        assert(grayscale_bitmap.get_height() == m_size.h);
        assert(grayscale_bitmap.get_width() == m_size.w);
        assert(grayscale_bitmap.get_channels() == 1);

        uint32_t* rbga_pixels = m_pixels;
        for (uint8_t pixel : grayscale_bitmap)
        {
            uint8_t r = pixel;
            uint8_t g = pixel;
            uint8_t b = pixel;
            uint8_t a = pixel;

            *rbga_pixels++ = (a << 24) | (b << 16) | (g << 8) | (r << 0);
        }
    }

    /* ------------------------------------- Iterator ------------------------------------- */

    class Iterator
    {
        uint8_t* m_ptr;
        int m_channels;
    public:
        Iterator(void* pixel_ptr, int channels)
        : m_ptr(static_cast<uint8_t*>(pixel_ptr))
        , m_channels(channels)
        {}

        Iterator operator++()
        {
            Iterator iterator = *this;
            m_ptr += m_channels;
            return iterator;
        }

        T& operator*() 
        { 
            return *m_ptr; 
        }

        bool operator!=(const Iterator& rhs)
        {
            return m_ptr != rhs.m_ptr;
        }
    };

    Iterator begin()
    {
        return Iterator(m_pixels, m_channels);
    }

    Iterator end()
    {
        uint8_t* pixels_ptr = static_cast<uint8_t*>(m_pixels);
        int end_index = m_size.w * m_size.h * m_channels;
        uint8_t* pixels_end = &pixels_ptr[end_index];
        return Iterator(pixels_end, m_channels);
    }
};

class Font
{
    const char* m_filepath;
    int m_first_codepoint;
    int m_last_codepoint;
    
    Bitmap<uint32_t>& m_bitmap;
    float m_font_size;
    Array<stbtt_packedchar> m_packedchars;

public:
    Font(Bitmap<uint32_t>& bitmap, int codepoint_range[2], float font_size, const char* filepath);
    ~Font();
    stbtt_packedchar get_glyph(char c);
    float get_font_size();

    Bitmap<uint32_t>& get_bitmap();
};

class Text
{
    // TODO: Remove limit and allocate text storage.
    static const int TEXT_MAX = 128;

    Font& m_font;
    struct { int w, h; } m_size;
    int m_length;
    float m_text_size;

    Array<Vec4<float>> m_glyph_rects;
    Array<Vec4<float>> m_glyph_tex_coords;

public:
    Text(Font& font, float text_size, const char* format, ...);
    Text(Font& font, float text_size, const char* format, va_list va_list);
    Text(const Text& text);
    void operator=(const Text& other);
    void adjust_text(float x, float y);
    int get_length();

    Array<Vec4<float>>& get_glyph_rects();
    Array<Vec4<float>>& get_glyph_tex_coords();

    Vec4<float> get_glyph_rect(size_t index);
    Vec4<float> get_glyph_tex_coord(size_t index);

private:
    static Text create_text_from_va_list(Font& font, float text_size, const char* format, va_list va_list);
};

struct Vertex
{
    Vec2<float> position;
    Color color;
    Vec2<float> tex_coords;
};

enum class QuadType
{
    QUAD_COLORED,
    QUAD_TEXTURED,
    QUAD_TEXT
};

static const int VERTCIES_PER_QUAD = 6;
struct Quad
{
    Vertex vertices[6];
};

class Renderer
{
    GLuint m_vertex_array;
    GLuint m_vertex_buffer;
    GLuint m_program;
    GLuint m_texture;
    Font& m_font;
    
    struct { float w, h; } m_frame_size;

    // TODO: What should this be?
    static const int QUAD_BUFFER_CAPACITY = 1024;
    Array<Quad> m_colored_quads;
    Array<Quad> m_textured_quads;
    Array<Quad> m_text_quads;

public:
    Renderer(Font& font);
    void init();

    void draw_rect(Vec4<float> rect, Color color, const char* filepath, Vec4<float> tex_coords, QuadType type);
    void draw_rect(Vec4<float>, Color color);
    void draw_rect(Vec4<float>, const char* filepath);
    void draw_text(float x, float y, float text_size, const char* format, ...);
    void draw_text(float x, float y, Text& text);

    void set_font(Font& font);
    void set_frame_size(float w, float h);
    void clear(Color color);
    
    GLint get_uniform_location(const char* name);

    /* ----------------------------- Uniforms specializations ----------------------------- */
    template<typename T>
    void set_uniform(const char* name, T value);

    template<>
    void set_uniform<bool>(const char* name, bool value)
    {
        GLint location = get_uniform_location(name);
        glUniform1i(location, value);
    }

    template<>
    void set_uniform<Vec2<float>>(const char* name, Vec2<float> value)
    {
        float* value_as_array = (float*) &value;
        GLint location = get_uniform_location(name);
        glUniform2f(location, value_as_array[0], value_as_array[1]);
    }

    void flush();

private:
    void push_quad_colored(Quad quad);
    void push_quad_textured(Quad quad);
    void push_quad_text(Quad quad);

    void flush_colored();
    void flush_textured();
    void flush_text();
    
    const char* get_shader_type_string(GLenum type);
    GLuint create_shader(const char* source, GLenum type);
    GLuint create_program(const char* vertex_source, const char* fragment_source);
};
