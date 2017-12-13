#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 color;

void main() {
	color = (texture(sprite,TexCoords)* 0.6 * ParticleColor);
}