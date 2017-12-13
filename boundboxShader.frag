#version 330 core
out vec4 color;

uniform vec3 box_color;

void main()
{
    color = vec4(box_color.r, box_color.g, 0.0f, 1.0f);
}