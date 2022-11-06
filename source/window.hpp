#pragma once

class Window
{
    class SDL_Window* m_handle;
    const char* m_title;
    struct { int x, y; } m_position;
    struct { int w, h; } m_size;
    bool m_open;

    class Renderer& m_renderer;
    
public:
    Window(const char* title, int x, int y, int w, int h, class Renderer& renderer);

    bool is_open();
    void swap_buffers();
    void poll_events();
    int get_width();
    int get_height();
};
