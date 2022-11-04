#include "renderer.hpp"
#include "shaders.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

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

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*) 0);

    m_program = create_program(vertex_source, fragment_source);
    glUseProgram(m_program);
}

void Renderer::draw_triangle()
{
    float triangle[] = 
    {
        +0.0f, +0.5f,
        -0.5f, -0.5f,
        +0.5f, -0.5f,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), &triangle, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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

void Renderer::draw_rect(float x, float y, float w, float h)
{
    float x0 = x;
    float x1 = x0 + w;
    float y0 = y;
    float y1 = y + h;

    float rect[] =
    {
        x0, y0,
        x1, y0,
        x0, y1,

        x1, y0,
        x0, y1,
        x1, y1,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), &rect, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

void Renderer::set_uniform_vec2(const char* name, float value[2])
{
    GLint location = glGetUniformLocation(m_program, name);
    assert(location != -1);
    glUniform2f(location, value[0], value[1]);
}
