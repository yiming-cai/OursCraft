#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 TexCoords;
in vec3 pos;
uniform int id;
//in vec3 positionColor;
// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

uniform samplerCube skybox;

// this is for the fog
uniform int disableFog;
uniform vec3 fog_color;
uniform float fog_end;
uniform float fog_start;
uniform vec3 fog_pos;

void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
	color = texture(skybox,TexCoords);

	if(disableFog == 0) {
		vec3 delta = vec3(mat4(1.0f) * vec4(pos,1.0f)) - fog_pos;
		delta.y = 0;
		float fog_f = (fog_end - sqrt(dot(delta,delta))) / (fog_end - fog_start);
		if(fog_f  < 0) fog_f  = 0;
		if(fog_f  > 1.0) fog_f  = 1.0;
		color = fog_f  * color + vec4((1.0 - fog_f ) * fog_color,0.0f);
		color.w = 1;
	}
}
