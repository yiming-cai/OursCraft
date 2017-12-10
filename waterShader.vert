#version 330 core

layout (location = 0) in vec3 position_in;
layout (location = 1) in vec3 normal_in;
layout (location = 2) in vec3 color;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float size;

out vec3 positionColor;
out vec2 TexCoords;
out vec3 position;
out vec3 normal;


void main()
{
    gl_Position = projection *  view * model * vec4(position_in, 1.0);
	position = position_in;
	normal = normal_in;
	TexCoords = vec2(position_in.x / size,position_in.z / size);
	positionColor = color;
}
