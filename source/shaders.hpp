static const char* vertex_source = R"(
#version 300 es

in vec2 position;
in vec4 color;
in vec2 tex_coords;

out vec4 frag_color;
out vec2 frag_tex_coords;

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
    frag_tex_coords = tex_coords;
}

)";

static const char* fragment_source = R"(
#version 300 es
precision mediump float;

in vec4 frag_color;
in vec2 frag_tex_coords;

out vec4 color;

uniform bool is_textured;

uniform sampler2D sampler;

void main()
{
    if (is_textured)
        color = texture(sampler, frag_tex_coords) * frag_color;
    else
        color = frag_color;
}

)";
