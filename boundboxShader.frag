#version 330 core
out vec4 color;

uniform vec3 collision_color;

void main()
{
    color = vec4(collision_color.r, collision_color.g, 0.0f, 1.0f);
}