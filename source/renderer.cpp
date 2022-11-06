#include "renderer.hpp"
#include "shaders.hpp"
#include "utils.hpp"

Font::Font(Bitmap<uint32_t>& bitmap, int codepoint_range[2], float font_size, const char* filepath)
: m_bitmap(bitmap)
, m_font_size(font_size)
, m_filepath(filepath)
, m_first_codepoint(codepoint_range[0])
, m_last_codepoint(codepoint_range[1])
{

    int grayscale_channels      = 1;
    int grayscale_bitmap_width  = m_bitmap.get_width();
    int grayscale_bitmap_height = m_bitmap.get_height();
    Bitmap<uint8_t> grayscale_bitmap(grayscale_bitmap_width, grayscale_bitmap_height, grayscale_channels);

    stbtt_pack_context pack_context        = {};
    uint8_t* grayscale_bitmap_pixels = static_cast<uint8_t*>(grayscale_bitmap.get_pixel_buffer());
    int grayscale_bitmap_stride            = grayscale_bitmap.get_stride();
    int padding                            = 1;

    stbtt_PackBegin(&pack_context,
                    grayscale_bitmap_pixels,
                    grayscale_bitmap_width,
                    grayscale_bitmap_height,
                    grayscale_bitmap_stride,
                    padding,
                    nullptr);

    File font_file(filepath);;
    uint8_t* font_data = static_cast<uint8_t*>(font_file.get_data());
    int font_index           = 0;
    int num_of_codepoints    = m_last_codepoint - m_first_codepoint + 1;
    m_packedchars.resize(num_of_codepoints);

    stbtt_PackFontRange(&pack_context,
                        font_data,
                        font_index,
                        font_size,
                        m_first_codepoint,
                        num_of_codepoints,
                        m_packedchars.get_underlying_buffer());
    
    stbtt_PackEnd(&pack_context);

    m_bitmap.copy_grayscale_as_rgba(grayscale_bitmap);
}

Font::~Font()
{
    memset(this, 0, sizeof(*this));
}

Bitmap<uint32_t>& Font::get_bitmap()
{
    return m_bitmap;
}

Text::Text(Font& font, float text_size, const char* format, ...)
: m_font(font)
{
    va_list va_list;
    va_start(va_list, format);
    *this = Text(font, text_size, format, va_list);
    va_end(va_list);
}

Text::Text(Font& font, float text_size, const char* format, va_list va_list)
: m_text_size(text_size)
, m_font(font)
{
    Array<char> buffer(TEXT_MAX);
    m_length = vsnprintf(buffer.get_underlying_buffer(), TEXT_MAX-1, format, va_list);
    m_glyph_rects.resize(m_length);
    m_glyph_tex_coords.resize(m_length);

    float x       = 0;
    float y       = 0;
    float scaling = text_size / font.get_font_size();

    // TODO: If we support unicode then we'll have to alter this slightly.
    for (size_t i = 0; i < m_length; i++) 
    {
        char character         = buffer[i];
        stbtt_packedchar glyph = m_font.get_glyph(character);

        float glyph_x_offset   = glyph.xoff * scaling;
        float glyph_y_offset   = glyph.yoff * scaling;
        float glyph_width      = (glyph.x1 - glyph.x0) * scaling;
        float glyph_height     = (glyph.y1 - glyph.y0) * scaling;
        float glyph_x_advance  = glyph.xadvance * scaling;

        /* ------------------------------- Glyph screen position ------------------------------ */
        Vec4<float> glyph_rect = {};

        glyph_rect.x0 = x + glyph_x_offset;
        glyph_rect.x1 = glyph_rect.x0 + glyph_width;
        x += glyph_x_advance;

        glyph_rect.y0 = y + glyph_y_offset;
        glyph_rect.y1 = glyph_rect.y0 + glyph_height;

        m_glyph_rects.push(glyph_rect);

        /* --------------------------- Glpyh texture sample position -------------------------- */
        Vec4<float> glyph_tex_coord = {};
        glyph_tex_coord.s0 = (float) glyph.x0 / font.get_bitmap().get_width();
        glyph_tex_coord.s1 = (float) glyph.x1 / font.get_bitmap().get_width();
        glyph_tex_coord.t0 = (float) glyph.y0 / font.get_bitmap().get_height();
        glyph_tex_coord.t1 = (float) glyph.y1 / font.get_bitmap().get_height();

        m_glyph_tex_coords.push(glyph_tex_coord);
    }

    Vec4<float> first_glyph = m_glyph_rects.first();
    Vec4<float> last_glyph  = m_glyph_rects.last();

    m_size.w                = last_glyph.x1 - first_glyph.x0;
    m_size.h                = text_size;
}

void Text::operator=(const Text& other)
{
    *this = Text(other);
}

Text::Text(const Text& text)
: m_font(text.m_font)
, m_size(text.m_size)
, m_length(text.m_length)
, m_text_size(text.m_text_size)
, m_glyph_rects(text.m_glyph_rects)
{ }

void Text::adjust_text(float x, float y)
{
    for (size_t i = 0; i < m_length; i++)
    {
        Vec4<float>& glyph_rect = m_glyph_rects[i];
        glyph_rect.x0 += x;
        glyph_rect.x1 += x;

        glyph_rect.y0 += y;
        glyph_rect.y1 += y;
    }
}

int Text::get_length()
{
    return m_length;
}

Array<Vec4<float>>& Text::get_glyph_rects()
{
    return m_glyph_rects;
}

Array<Vec4<float>>& Text::get_glyph_tex_coords()
{
    return m_glyph_tex_coords;
}

static void glewErrorAndExit(GLenum error_code)
{
    fprintf(stderr, "%s\n", glewGetErrorString(error_code));
    exit(EXIT_FAILURE);
}

Renderer::Renderer(Font& font)
: m_font(font)
, m_vertex_array(0)
, m_vertex_buffer(0)
, m_program(0)
, m_texture(0)
, m_frame_size({0, 0})
{ }

stbtt_packedchar Font::get_glyph(char c)
{
    int char_index = static_cast<int>(c);
    assert(char_index >= m_first_codepoint && char_index <= m_last_codepoint);
    return m_packedchars[char_index];
}

float Font::get_font_size()
{
    return m_font_size;
}

void Renderer::init()
{
    GLenum glew_init_result = glewInit();
    if (glew_init_result != GLEW_OK)
        glewErrorAndExit(glew_init_result);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &m_vertex_array);
    glBindVertexArray(m_vertex_array);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texture);

    // TODO: Make this code more robust so that the vertex buffer layout stays
    //       in sync with the "Vertex" type.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, tex_coords));

    m_program = create_program(vertex_source, fragment_source);
    glUseProgram(m_program);
}

void Renderer::set_frame_size(float w, float h)
{
    m_frame_size.w = w;
    m_frame_size.h = h;

    Vec2<float> frame_size = { m_frame_size.w, m_frame_size.h };
    set_uniform("frame_size", frame_size);
    glViewport(0, 0, w, h);
}

void Renderer::clear(Color color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw_rect(Vec4<float> rect, Color color, const char* filepath, Vec4<float> tex_coords, bool is_textured)
{
    if (filepath)
    {
        int desired_channels = 4;
        int image_w, image_h, image_channels;
        uint8_t* image_data = stbi_load(filepath, &image_w, &image_h, &image_channels, desired_channels);
        assert(image_data);
        assert(desired_channels == image_channels);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);
    }

    Quad quad = {};
    Vertex* vertices = quad;

    vertices[0].position = { rect.x0, rect.y0 };
    vertices[1].position = { rect.x1, rect.y0 };
    vertices[2].position = { rect.x0, rect.y1 };
    vertices[3].position = { rect.x1, rect.y0 };
    vertices[4].position = { rect.x0, rect.y1 };
    vertices[5].position = { rect.x1, rect.y1 };

    for (int i = 0; i < VERTCIES_PER_QUAD; i++)
        vertices[i].color = color;

    vertices[0].tex_coords = { tex_coords.s0, tex_coords.t0 };
    vertices[1].tex_coords = { tex_coords.s1, tex_coords.t0 };
    vertices[2].tex_coords = { tex_coords.s0, tex_coords.t1 };
    vertices[3].tex_coords = { tex_coords.s1, tex_coords.t0 };
    vertices[4].tex_coords = { tex_coords.s0, tex_coords.t1 };
    vertices[5].tex_coords = { tex_coords.s1, tex_coords.t1 };

    set_uniform("is_textured", is_textured);

    // TODO: Remove when batch rendering suppported
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, VERTCIES_PER_QUAD);

}

void Renderer::draw_rect(Vec4<float> rect, Color color)
{
    draw_rect(rect, color, nullptr, { 0, 0, 0, 0 }, false);
}

void Renderer::draw_rect(Vec4<float> rect, const char* filepath)
{
    // TODO: Find out what the best parameters to use here.
    //       This will be dependent on our assset packing strategy.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    draw_rect(rect, COLOR_WHITE, filepath, { 0, 0, 1, 1 }, true);
}

void Renderer::draw_text(float x, float y, float text_size ,const char* format, ...)
{
    va_list va_list;
    va_start(va_list, format);
    Text text = Text(m_font, text_size, format, va_list);
    va_end(va_list);

    draw_text(x, y, text);
}

void Renderer::draw_text(float x, float y, Text& text)
{
    // TODO: Find out what the best parameters to use here.
    //       This will be dependent on our assset packing strategy.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    text.adjust_text(x, y);
    Bitmap<uint32_t>& font_bitmap = m_font.get_bitmap();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_bitmap.get_width(), font_bitmap.get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, font_bitmap.get_pixel_buffer());

    Array<Vec4<float>>& glyph_rects       = text.get_glyph_rects();
    Array<Vec4<float>>& glyph_text_coords = text.get_glyph_tex_coords();

    for (size_t i = 0; i < text.get_length(); i++)
    {
        Vec4<float> glyph_rect = glyph_rects[i];
        Vec4<float> glyph_tex_coord = glyph_text_coords[i];
        draw_rect(glyph_rect, COLOR_WHITE, nullptr, glyph_tex_coord, true);
    }
}

const char* Renderer::get_shader_type_string(GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
        default: assert(false);  return "";
    }
}

GLuint Renderer::create_shader(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compile_success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_success);
    if (!compile_success)
    {
        constexpr int buffer_capacity = 1024;
        char buffer[buffer_capacity] = {};
        glGetShaderInfoLog(shader, buffer_capacity, nullptr, buffer);

        fprintf(stderr, "%s shader error: %s\n", get_shader_type_string(type), buffer);
        exit(EXIT_FAILURE);
    }

    return shader;
}

GLuint Renderer::create_program(const char* vertex_source, const char* fragment_source)
{
    GLuint vertex_shader   = create_shader(vertex_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = create_shader(fragment_source, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    return program;
}

GLint Renderer::get_uniform_location(const char* name)
{
    GLint location = glGetUniformLocation(m_program, name);
    assert(location != -1);
    return location;
}
