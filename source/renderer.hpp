
typedef unsigned int GLuint;
typedef unsigned int GLenum;

struct Color { float r, g, b, a; };

static const Color COLOR_RED   {1, 0, 0, 1};
static const Color COLO_GREEN  {0, 1, 0, 1};
static const Color COLOR_BLUE  {0, 0, 1, 1};
static const Color COLOR_WHITE {1, 1, 1, 1};
static const Color COLOR_BLACK {0, 0, 0, 1};

template<typename T>
union Vec2
{
    struct { T x, y; };
    struct { T w, h; };
};

struct Vertex
{
    Vec2<float> position;
    Color color;
};

static const int VERTCIES_PER_QUAD = 6;
typedef Vertex Quad[VERTCIES_PER_QUAD];

class Renderer
{
    GLuint m_vertex_array;
    GLuint m_vertex_buffer;
    GLuint m_program;
    
    struct { float w, h; } m_frame_size;

public:
    Renderer();
    void init();
    void draw_triangle();
    void draw_rect(float x, float y, float w, float h, Color color);
    void set_frame_size(float w, float h);
    void clear(Color color);

    // TODO: Template specialization
    void set_uniform_vec2(const char* name, float value[2]);

private:
    const char* get_shader_type_string(GLenum type);
    GLuint create_shader(const char* source, GLenum type);
    GLuint create_program(const char* vertex_source, const char* fragment_source);
};
