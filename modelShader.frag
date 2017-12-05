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


layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

// main loop
void main()
{

	// find normal in world coordinate
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal_world = normalize(normalMatrix * normal);
	// vec3 normal_world = normalize( vec3(view * model * vec4(normal, 0.0f) ) );

	// calculate the location of this fragment in world coordinates
	vec3 vert = vec3(model * vec4(position, 1.0f));

	// set the direction of light, from surface to light source
	vec3 surfaceToLight = normalize( vec3(1.0f, 1.0f, 1.0f) );

	// find the lightToSurface vector
	vec3 lightToSurface = normalize( -1.0f * surfaceToLight) ;

	// find the R vector
	vec3 R_vec = lightToSurface - (2 * dot(lightToSurface, normal_world)) * normal_world;
	R_vec = normalize(R_vec);

	// find the e vector
	vec3 e_vec = cam_pos - vert;
	e_vec = normalize(e_vec);

	vec3 amb = ambient.rgb;
	vec3 dif = diffuse.rgb;
	vec3 spec = specular.rgb;
	float shine = shininess;

	vec3 c_mat = dif * dot( surfaceToLight, normal_world ) / length(surfaceToLight) / length(normal_world);
	c_mat += spec * pow( dot(R_vec, e_vec), 128.0f * shininess);
	// c_mat += amb;

	// find the c_l, or light intensity
	vec3 c_l = vec3(0.9f, 0.9f, 0.9f);

	// output color
	color = vec4( c_mat * c_l + amb, 1.0f );
}
