#version 330 core

in vec3 positionColor;
in vec3 TexCoords;

uniform int selected;
uniform int haveTexture;
uniform samplerCube textureBox;

out vec4 color;


void main()
{
	color = vec4(positionColor.x,positionColor.y,positionColor.z, 1.0f);
	if(haveTexture == 1) color = texture(textureBox,TexCoords);
	if(selected == 1) color = vec4(positionColor.x * 1.2,positionColor.y * 0.8,positionColor.z * 0.8, 1.0f);;
}
