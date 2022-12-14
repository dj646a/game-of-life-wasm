#include "renderer.hpp"
#include "window.hpp"

/*
    TODOS:
    
    HIGH PRIORITY
    Renderer should batch draw calls.

    MEDIUM PRIORITY
    Create platform abstractions for:
        Keyboard management.
        Mouse management.
        
    LOW PRIORITY
    Renderer should handle z ordering gracefully.
*/

int main()
{
    // TODO: Someway to pre-determine the correct bitmap dimensions
    //       to hold the font glpyh data.
    int font_bitmap_width    = 1000;
    int font_bitmap_height   = 1000;
    int font_bitmap_channels = 4;
    Bitmap<uint32_t> font_bitmap(font_bitmap_width, font_bitmap_height, font_bitmap_channels);
    
    float font_size           = 100;
    const char* font_filepath = "./assets/JetBrainsMono-Regular.ttf";
    int codepoint_range[]     = {0, 127};
    Font font(font_bitmap, codepoint_range, font_size, font_filepath);

    Renderer renderer(font);
    
    const char* window_title = "Game of Life WASM";
    int window_x             = 1000;
    int window_y             = 100;
    int window_w             = 800;
    int window_h             = 450;

    Window window(window_title, window_x, window_y, window_w, window_h, renderer);

    // Must wait for the window to create a valid OpenGL context before initializing the 
    // renderer.
    float renderer_frame_width  = window.get_width();
    float renderer_frame_height = window.get_height();
    
    renderer.init();
    renderer.set_frame_size(renderer_frame_width, renderer_frame_height);

    while (window.is_open())
    {
        renderer.clear(COLOR_RED);
        renderer.draw_rect({ 300, 100, 400, 200 }, COLOR_BLACK);
        renderer.draw_rect({ 100, 100, 200, 200 }, "./assets/image.png");
        renderer.draw_text(100, 300, 50, "Hello, %s", "Bob");

        window.swap_buffers();
        window.poll_events();
    }

    printf("EXIT_SUCCESS\n");
    exit(EXIT_SUCCESS);
}
