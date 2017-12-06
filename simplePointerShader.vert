#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 projection;
uniform mat4 modelview;


out vec3 positionColor;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	positionColor = color;
}
