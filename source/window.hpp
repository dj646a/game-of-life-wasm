class Window
{
    class SDL_Window* m_handle;
    const char* m_title;
    struct { int x, y; } m_position;
    struct { int w, h; } m_size;
    bool m_open;
    
public:
    Window(const char* title, int x, int y, int w, int h);

    bool is_open();
    void swap_buffers();
    void poll_events();
};
