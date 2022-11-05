#include <cstdio>
#include <cstdlib>

#include "renderer.hpp"
#include "window.hpp"

/*
    TODOS:
    
    HIGH PRIORITY
    Renderer should support drawing text.
    Renderer should batch draw calls.
    Pragma once in header files.

    MEDIUM PRIORITY
    Create platform abstractions for:
        Keyboard management.
        Mouse management.
        
    LOW PRIORITY
    Renderer should handle z ordering gracefully.
*/

int main()
{
    Renderer renderer;
    
    const char* window_title = "Game of Life WASM";
    int window_x             = 1000;
    int window_y             = 100;
    int window_w             = 800;
    int window_h             = 450;

    Window window(window_title, window_x, window_y, window_w, window_h, renderer);

    // Must wait for the window to create a valid OpenGL context before initializing the 
    // renderer.
    renderer.init();
    renderer.set_frame_size(window.get_width(), window.get_height());

    while (window.is_open())
    {
        renderer.clear(COLOR_RED);
        renderer.draw_rect(300, 100, 100, 100, COLOR_BLACK);
        renderer.draw_rect(100, 100, 100, 100, "./assets/image.png");

        window.swap_buffers();
        window.poll_events();
    }

    printf("EXIT_SUCCESS\n");
    exit(EXIT_SUCCESS);
}
