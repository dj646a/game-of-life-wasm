typedef unsigned int GLuint;

struct Color { float r, g, b, a; };

static const Color COLOR_RED   {1, 0, 0, 1};
static const Color COLO_GREEN  {0, 1, 0, 1};
static const Color COLOR_BLUE  {0, 0, 1, 1};
static const Color COLOR_WHITE {1, 1, 1, 1};
static const Color COLOR_BLACK {0, 0, 0, 1};

class Renderer
{
    GLuint m_vertex_array;
    GLuint m_vertex_buffer;
    
    struct { float w, h; } m_frame_size;

public:
    void init();
    void draw_triangle();
    void draw_rect(float x, float y, float w, float h);
    void set_frame_size(float w, float h);
    void clear(Color color);
};
