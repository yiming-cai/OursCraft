#version 330 core

in vec3 positionColor;

out vec4 color;


void main()
{
	color = vec4(positionColor.x,positionColor.y,positionColor.z, 1.0f);
}
