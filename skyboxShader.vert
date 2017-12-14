#version 330 core

// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;

uniform float distance;

uniform mat4 projection;
uniform mat4 modelview;

out vec3 TexCoords;
out vec3 pos;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
	TexCoords = vec3(position.x * distance,position.y * distance, position.z * distance);
    gl_Position = projection * modelview * vec4(position.x * distance ,position.y * distance, position.z * distance, 1.0);
	pos = position;
}
