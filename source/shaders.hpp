static const char* vertex_source = R"(
#version 300 es

in vec2 position;

void main()
{
    gl_Position = vec4(position, 1, 1);
}

)";

static const char* fragment_source = R"(
#version 300 es
precision mediump float;

out vec4 color;

void main()
{
    // Green
    color = vec4(0, 1, 0, 1);
}

)";
