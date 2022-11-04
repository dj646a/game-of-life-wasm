#include "renderer.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

// TODO: Remove when "project base" file done.
#include <cstdio>
#include <cstdlib>

static void glewErrorAndExit(GLenum error_code)
{
    fprintf(stderr, "%s\n", glewGetErrorString(error_code));
    exit(EXIT_FAILURE);
}

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
