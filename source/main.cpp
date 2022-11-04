#include <cstdio>
#include <cstdlib>

#include "renderer.hpp"
#include "window.hpp"

/*
    TODOS:

    Create platform abstractions for:
        Keyboard management.
        Mouse management.
*/

int main()
{
    const char* window_title = "Game of Life WASM";
    int window_x             = 1000;
    int window_y             = 100;
    int window_w             = 800;
    int window_h             = 450;

    Renderer renderer;
    Window window(window_title, window_x, window_y, window_w, window_h, renderer);

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
