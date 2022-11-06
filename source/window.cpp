#include "window.hpp"
#include "renderer.hpp"

static void SDL_ErrorAndExit()
{
    fprintf(stderr, "%s\n", SDL_GetError());
    exit(EXIT_FAILURE);
}

Window::Window(const char* title, int x, int y, int w, int h, Renderer& renderer)
: m_handle(nullptr)
, m_title(title)
, m_position({x, y})
, m_size({w, h})
, m_open(false)
, m_renderer(renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        SDL_ErrorAndExit();

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    
    uint32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    m_handle = SDL_CreateWindow(m_title,
                                m_position.x,
                                m_position.y,
                                m_size.w,
                                m_size.h,
                                window_flags);
                                            
    if (!m_handle) 
        SDL_ErrorAndExit(); 

    SDL_GLContext context = SDL_GL_CreateContext(m_handle);
    if (!context)
        SDL_ErrorAndExit();

    m_open = true;
}

bool Window::is_open()
{
    return m_open;
}

void Window::swap_buffers()
{
    SDL_GL_SwapWindow(m_handle);
}

void Window::poll_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                m_open = false;
            } break;

            case SDL_KEYDOWN:
            {
                SDL_Keycode keycode = event.key.keysym.sym;
                switch (keycode)
                {
                    case SDLK_ESCAPE:
                    {
                        m_open = false;
                    } break;
                }
            } break;

            case SDL_WINDOWEVENT:
            {
                uint8_t window_event = event.window.event;
                switch (window_event)
                {
                    // TODO: What's the difference between these two?
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        int window_w, window_h;
                        SDL_GetWindowSize(m_handle, &m_size.w, &m_size.h);

                        // TODO: Revise once we've decided how to size our frame relative
                        //       to our windows.
                        m_renderer.set_frame_size(m_size.w, m_size.h);
                    } break;
                }
            } break;
        }
    }
}

int Window::get_width()
{
    return m_size.w;
}

int Window::get_height()
{
    return m_size.h;
}
