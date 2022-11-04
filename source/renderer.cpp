#include "renderer.hpp"
#include "shaders.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <stb/stb_image.h>

// TODO: Remove when "project base" file done.
#include <cstdio>
#include <cstdlib>
#include <cassert>

static void glewErrorAndExit(GLenum error_code)
{
    fprintf(stderr, "%s\n", glewGetErrorString(error_code));
    exit(EXIT_FAILURE);
}

Renderer::Renderer()
: m_vertex_array(0)
, m_vertex_buffer(0)
, m_program(0)
, m_texture(0)
, m_frame_size({0, 0})
{ }

void Renderer::init()
{
    GLenum glew_init_result = glewInit();
    if (glew_init_result != GLEW_OK)
        glewErrorAndExit(glew_init_result);

    glEnable(GL_MULTISAMPLE);

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

    set_uniform_vec2("frame_size", (float*) &m_frame_size);
    glViewport(0, 0, w, h);
}

void Renderer::clear(Color color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw_rect(float x, float y, float w, float h, Color color)
{
    // TODO: Deduplicate code in draw_rect functions.

    float x0 = x;
    float x1 = x0 + w;
    float y0 = y;
    float y1 = y + h;

    Quad quad = {};
    Vertex* vertices = quad;

    vertices[0].position = { x0, y0 };
    vertices[1].position = { x1, y0 };
    vertices[2].position = { x0, y1 };
    vertices[3].position = { x1, y0 };
    vertices[4].position = { x0, y1 };
    vertices[5].position = { x1, y1 };

    for (int i = 0; i < VERTCIES_PER_QUAD; i++)
        vertices[i].color = color;

    bool is_textured = false;
    set_uniform_bool("is_textured", &is_textured);

    // TODO: Remove when batch rendering suppported
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, VERTCIES_PER_QUAD);
}

void Renderer::draw_rect(float x, float y, float w, float h, const char* filepath)
{
    // TODO: Deduplicate code in draw_rect functions.

    // TODO: Remove when asset strategy is finalized.
    int desired_channels = 4;
    int image_w, image_h, image_channels;
    unsigned char* image_data = stbi_load(filepath, &image_w, &image_h, &image_channels, desired_channels);
    assert(image_data);
    assert(desired_channels == image_channels);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    stbi_image_free(image_data);

    // TODO: What should these be?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float x0 = x;
    float x1 = x0 + w;
    float y0 = y;
    float y1 = y + h;

    Quad quad = {};
    Vertex* vertices = quad;

    vertices[0].position = { x0, y0 };
    vertices[1].position = { x1, y0 };
    vertices[2].position = { x0, y1 };
    vertices[3].position = { x1, y0 };
    vertices[4].position = { x0, y1 };
    vertices[5].position = { x1, y1 };

    for (int i = 0; i < VERTCIES_PER_QUAD; i++)
        vertices[i].color = COLOR_WHITE;

    float s0 = 0;
    float s1 = 1;
    float t0 = 0;
    float t1 = 1;

    vertices[0].tex_coords = { s0, t0 };
    vertices[1].tex_coords = { s1, t0 };
    vertices[2].tex_coords = { s0, t1 };
    vertices[3].tex_coords = { s1, t0 };
    vertices[4].tex_coords = { s0, t1 };
    vertices[5].tex_coords = { s1, t1 };

    bool is_textured = true;
    set_uniform_bool("is_textured", &is_textured);

    // TODO: Remove when batch rendering suppported
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, VERTCIES_PER_QUAD);
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

void Renderer::set_uniform_vec2(const char* name, float value[2])
{
    GLint location = get_uniform_location(name);
    glUniform2f(location, value[0], value[1]);
}

void Renderer::set_uniform_bool(const char* name, bool* value)
{
    GLint location = get_uniform_location(name);
    glUniform1i(location, *value);
}
