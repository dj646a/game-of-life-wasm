#include <cstdio>
#include <cstdlib>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "window.hpp"

/*
    TODOS:
        Create platform abstractions for:
            Keyboard management.
            Mouse management.

        Create renderer abstraction.
*/

static void glewErrorAndExit(GLenum error_code)
{
    fprintf(stderr, "%s\n", glewGetErrorString(error_code));
    exit(EXIT_FAILURE);
}

int main()
{
    const char* window_title = "Game of Life WASM";
    int window_x             = 1000;
    int window_y             = 100;
    int window_w             = 800;
    int window_h             = 450;

    Window window(window_title, window_x, window_y, window_w, window_h);

    GLenum glew_init_result = glewInit();
    if (glew_init_result != GLEW_OK)
        glewErrorAndExit(glew_init_result);

    glEnable(GL_MULTISAMPLE);

    float triangle[] = 
    {
        +0.0f, +0.5f,
        -0.5f, -0.5f,
        +0.5f, -0.5f,
    };

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), &triangle, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*) 0);

    while (window.is_open())
    {
        /* --------------------------------- OpenGL rendering --------------------------------- */
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.swap_buffers();
        window.poll_events();
    }

    printf("EXIT_SUCCESS\n");
    exit(EXIT_SUCCESS);
}
