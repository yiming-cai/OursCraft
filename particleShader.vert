#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform vec4 color;
uniform vec3 offset;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

void main(){
	TexCoords = texCoord;
	ParticleColor = color;
	gl_Position = projection * modelview * vec4((vertex.xyz * 0.1f) + offset, 1.0);
}