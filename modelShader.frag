#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 normal;
in vec3 position;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

// projection matrix
uniform mat4 projection;

// model to view matrix
uniform mat4 model;

// view to world matrix
uniform mat4 view;

// camera position
uniform vec3 cam_pos;

// main loop
void main()
{

	// For testing
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	return;

	// find normal in world coordinate
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal_world = normalize(normalMatrix * normal);

	// calculate the location of this fragment in world coordinates
	vec3 vert = vec3(model * vec4(position, 1.0f));

	vec3 surfaceToLight = vec3(1.0f);

	// find the lightToSurface vector
	vec3 lightToSurface = -1.0f * surfaceToLight;

	// find the R vector
	vec3 R_vec = lightToSurface - (2 * dot(lightToSurface, normal_world)) * normal_world;
	R_vec = normalize(R_vec);

	// find the e vector
	vec3 e_vec = cam_pos - vert;
	e_vec = normalize(e_vec);

	// find the c_l, or light intensity
	vec3 c_l = vec3(0.5f, 0.8f, 0.3f);

	color = vec4(c_l, 1.0f);

}
