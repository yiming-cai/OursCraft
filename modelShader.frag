#version 330 core

// ----------------------------------------------------------------------------------------------------------------------------------
// Define the light struct
struct Light {
	vec3 position; // also used as direction for directional light
	vec3 intensities; // a.k.a the color of the light
	vec3 coneDirection; // only needed for spotlights

	float attenuation; // only needed for point and spotlights
	float ambientCoefficient; // how strong the light ambience should be... 0 if there's no ambience (background reflection) at all
	float coneAngle; // only needed for spotlights
	float exponent; // cosine exponent for how light tapers off
	int type; // specify the type of the light (directional, spotlight, point light)
	int attenuationType; // specify the type of attenuation to use
	int status;			// 0 for turning off the light, 1 for turning on the light
};

// some constants for the light structures
const int TYPE_DIRECTIONAL = 0;
const int TYPE_POINT = 1;
const int TYPE_SPOT = 2;

const int ATTENUATION_CONSTANT = 0;
const int ATTENUATION_LINEAR = 1;
const int ATTENUATION_QUADRATIC = 2;

const int STATUS_ON = 0;
const int STATUS_OFF = 1;

const int NUM_LIGHTS = 16;
// ----------------------------------------------------------------------------------------------------------------------------------

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 normal;
in vec3 position;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

// some matrices and camera position
uniform mat4 projection;	
uniform mat4 model;
uniform mat4 view;
uniform vec3 cam_pos;


// get the Material properties
layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};


// get the array of lights
layout (std140) uniform LightBlock {
	Light lights[NUM_LIGHTS];
};


// main loop
void main()
{
	// find normal in world coordinate
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal_world = normalize(normalMatrix * normal);

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

	if (texCount == 0)
	{
		vec4 amb = ambient;
		vec4 dif = diffuse;
		vec4 spec = specular;
		float shine = shininess;

		float nDotL = dot( normal_world, surfaceToLight );

		vec4 c_mat = dif * max( 0.0f, nDotL );
		c_mat += (nDotL == 0.0f) ? vec4(0.0f) : spec * max(0.0f, pow( dot(R_vec, e_vec), shininess));
		c_mat += dif * amb;

		// find the c_l, or light intensity
		vec4 c_l = vec4(0.9f, 0.9f, 0.9f, 1.0f);

		// output color
		color = vec4( c_mat * c_l );
	}
	else
	{
		// If there's a texture....
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
}
