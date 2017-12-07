#version 330 core

// ----------------------------------------------------------------------------------------------------------------------------------
// Define the light struct
struct Light {
	vec4 position; // also used as direction for directional light
	vec4 intensities; // a.k.a the color of the light
	vec4 coneDirection; // only needed for spotlights

	float attenuation; // only needed for point and spotlights
	float ambientCoefficient; // how strong the light ambience should be... 0 if there's no ambience (background reflection) at all
	float coneAngle; // only needed for spotlights
	float exponent; // cosine exponent for how light tapers off
	int type; // specify the type of the light (directional, spotlight, point light)
	int attenuationType; // specify the type of attenuation to use
	int status;			// 0 for turning off the light, 1 for turning on the light
	int PADDING;		// useless, don't touch it
};

// some constants for the light structures
const int TYPE_DIRECTIONAL = 0;
const int TYPE_POINT = 1;
const int TYPE_SPOT = 2;

const int ATTENUATION_CONSTANT = 0;
const int ATTENUATION_LINEAR = 1;
const int ATTENUATION_QUADRATIC = 2;

const int STATUS_OFF = 0;
const int STATUS_ON = 1;

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
	// records the sum of light colors;
	vec4 sum_of_colors = vec4(0);

	// find normal in world coordinate
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal_world = normalize(normalMatrix * normal);

	// the material constants
	vec4 amb = ambient;
	vec4 dif = diffuse;
	vec4 spec = specular;
	float shine = shininess;

	// calculate the location of this fragment in world coordinates
	vec3 vert = vec3(model * vec4(position, 1.0f));

	// find the e vector, the direction of ray towards the camera
	// since this is not dependent on light, can calculate first
	vec3 e_vec = cam_pos - vert;
	e_vec = normalize(e_vec);

	for (int light_i = 0; light_i < NUM_LIGHTS; light_i++)
	{	
		if (lights[light_i].status == STATUS_OFF) continue;

		// direction of light relative to the vertex
		vec3 surfaceToLight;
		vec3 lightToSurface;
		vec3 R_vec;
		float nDotL;
		vec4 c_mat;
		vec4 c_l;
		float dist;
		float attenuation_val;

		// find the direction of light
		if (lights[light_i].type == TYPE_DIRECTIONAL)
		{
			lightToSurface = normalize(lights[light_i].position.xyz);
			surfaceToLight = normalize( -1.0f * lightToSurface );
		}
		else if (lights[light_i].type == TYPE_POINT || lights[light_i].type == TYPE_SPOT)
		{
			surfaceToLight = normalize( lights[light_i].position.xyz - vert );
			lightToSurface = normalize( -1.0f * surfaceToLight );
		}

		// find the light intensity based on the type of attenuation
		if (lights[light_i].attenuationType == ATTENUATION_CONSTANT)
		{
			attenuation_val = lights[light_i].attenuation;
		}
		else if (lights[light_i].attenuationType == ATTENUATION_LINEAR)
		{
			attenuation_val = lights[light_i].attenuation * length(lights[light_i].position.xyz - vert);
		}
		else if (lights[light_i].attenuationType == ATTENUATION_QUADRATIC)
		{
			dist = length(lights[light_i].position.xyz - vert);
			attenuation_val = lights[light_i].attenuation * dist * dist ;
		}
		else
		{
			attenuation_val = 0.0f;
		}

		// find the angle between the normal and the light vectors
		nDotL = dot( normal_world, surfaceToLight );

		// find the light intensity after attenuation
		if (lights[light_i].type == TYPE_DIRECTIONAL || lights[light_i].type == TYPE_POINT)
		{
			c_l = lights[light_i].intensities / (1.0f + attenuation_val);
		}
		else if (lights[light_i].type == TYPE_SPOT)
		{
			float lxd = dot(lightToSurface, lights[light_i].coneDirection.xyz);
			if (lxd <= cos( lights[light_i].coneAngle )) 
			{
				c_l = vec4(0.0f);
			}
			else
			{
				c_l = vec4(lights[light_i].intensities.xyz * pow(lxd, lights[light_i].exponent) / (1.0f + attenuation_val), 1.0f);
			}
		}

		// find the R vector, the direction of reflected ray
		R_vec = lightToSurface - (2 * dot(lightToSurface, normal_world)) * normal_world;
		R_vec = normalize(R_vec);

		// if there is no texture....
		if (texCount == 0)
		{
			c_mat = dif * max( 0.0f, nDotL );
			c_mat += (nDotL == 0.0f) ? vec4(0.0f) : spec * max(0.0f, pow( dot(R_vec, e_vec), shininess));
			c_mat += dif * lights[light_i].ambientCoefficient * amb;
		}

		// else if there is a texture.... MAKE IT RED FOR NOW 
		else
		{
			c_mat = vec4(1.0f, 0, 0, 0);
		}

		sum_of_colors += c_l * c_mat;
	}

	color = vec4(sum_of_colors.xyz, 1.0f);
}
