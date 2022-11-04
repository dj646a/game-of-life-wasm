static const char* vertex_source = R"(
#version 300 es

in vec2 position;
in vec4 color;

out vec4 frag_color;

uniform vec2 frame_size;

vec2 frame_to_gl_space(vec2 point)
{
    vec2 result = point / frame_size;
    result = result * 2.0f - 1.0f;
    result.y *= -1.0f;
    return result;
}

void main()
{
    vec2 position_in_gl_space = frame_to_gl_space(position);
    gl_Position = vec4(position_in_gl_space, 1, 1);

    frag_color = color;
}

)";

static const char* fragment_source = R"(
#version 300 es
precision mediump float;

in vec4 frag_color;

out vec4 color;

void main()
{
    // Green
    color = frag_color;
}

)";
