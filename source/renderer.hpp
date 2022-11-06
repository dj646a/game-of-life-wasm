#include <cstdarg>

#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

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

class Bitmap
{
    void* m_pixels;
    struct { int w, h; } m_size;
    int m_channels;
    int m_stride;

public:
    Bitmap(int width, int height, int channels);
    ~Bitmap();

    void* get_pixel_buffer();
    unsigned char* get_pixel_buffer_as_u8();
    int get_width();
    int get_height();
    int get_stride();
    int get_channels();
    void copy_grayscale_as_rgba(Bitmap& grayscale_bitmap);
};

class Font
{
    const char* m_filepath;
    int m_first_codepoint;
    int m_last_codepoint;
    
    Bitmap& m_bitmap;
    float m_font_size;
    Array<stbtt_packedchar> m_packedchars;

public:
    Font(Bitmap& bitmap, int codepoint_range[2], float font_size, const char* filepath);
    ~Font();
    stbtt_packedchar get_glyph(char c);
    float get_font_size();

    Bitmap& get_bitmap();
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

static const int VERTCIES_PER_QUAD = 6;
typedef Vertex Quad[VERTCIES_PER_QUAD];

class Renderer
{
    GLuint m_vertex_array;
    GLuint m_vertex_buffer;
    GLuint m_program;
    GLuint m_texture;
    Font& m_font;
    
    struct { float w, h; } m_frame_size;

public:
    Renderer(Font& font);
    void init();
    void draw_rect(float x, float y, float w, float h, Color color);
    void draw_rect(float x, float y, float w, float h, const char* filepath);
    void draw_text(float x, float y, float text_size, const char* format, ...);
    void draw_text(float x, float y, Text& text);

    void set_font(Font& font);
    void set_frame_size(float w, float h);
    void clear(Color color);

    // TODO: Template specialization
    GLint get_uniform_location(const char* name);
    void set_uniform_vec2(const char* name, float value[2]);
    void set_uniform_bool(const char* name, bool* value);

private:
    const char* get_shader_type_string(GLenum type);
    GLuint create_shader(const char* source, GLenum type);
    GLuint create_program(const char* vertex_source, const char* fragment_source);
};
