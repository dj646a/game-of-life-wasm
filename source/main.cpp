#include <cstdio>
#include <cstdlib>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

/*
    TODOS:
        Create platform abstractions for:
            Window management.
            Keyboard management.
            Mouse management.

        Create renderer abstraction.
*/

static void SDL_ErrorAndExit()
{
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(EXIT_FAILURE);
}

static void glewErrorAndExit(GLenum error_code)
{
    fprintf(stderr, "%s\n", glewGetErrorString(error_code));
    exit(EXIT_FAILURE);
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        SDL_ErrorAndExit();

    bool window_open         = false;
    const char* window_title = "Game of Life WASM";
    int window_x             = 1000;
    int window_y             = 100;
    int window_w             = 800;
    int window_h             = 450;
    uint32_t window_flags    = SDL_WINDOW_OPENGL;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    SDL_Window* window_handle = SDL_CreateWindow(window_title, window_x, window_y, window_w, window_h, window_flags);
    if (!window_handle)
        SDL_ErrorAndExit();

    SDL_GLContext context = SDL_GL_CreateContext(window_handle);
    if (!context)
        SDL_ErrorAndExit();

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

    window_open = true;
    while (window_open)
    {
        /* --------------------------------- OpenGL rendering --------------------------------- */
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window_handle);

        /* ------------------------------- Handle window events ------------------------------- */
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    window_open = false;
                } break;

                case SDL_KEYDOWN:
                {
                    SDL_Keycode keycode = event.key.keysym.sym;
                    switch (keycode)
                    {
                        case SDLK_ESCAPE:
                        {
                            window_open = false;
                        } break;
                    }
                } break;
            }
        }
    }

    printf("EXIT_SUCCESS\n");
    exit(EXIT_SUCCESS);
}
