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

// to render in normal colors or under lights
uniform int normal_mode;
uniform int light_mode;

// material properties
uniform vec3 k_s;
uniform vec3 k_d;
uniform vec3 k_a;
uniform float shininess;

// whether currently drawing skybox or not
uniform int draw_skybox;
uniform samplerCube skybox;
uniform sampler2D backbuffer;
uniform vec2 resolution;
uniform int draw_sphere;

// For drawing a line segment
uniform int draw_line;
uniform vec3 line_color;

// For makine an environment map
uniform int environment_map;

// camera position
uniform vec3 cam_pos;

// light properties
uniform struct DirectionalLight
{
	vec3 light_color;
	vec3 light_direction;
} dir_l;

uniform struct PointLight
{
	vec3 light_color;
	vec3 light_position;
	float attenuation; // attenuation constant
} point_l;

uniform struct SpotLight
{
	vec3 light_color; 
	vec3 light_position; // if w component=0 it’s directional
	float attenuation; // only needed for point and spotlights
	vec3 cone_direction; // only needed for spotlights
	float cone_angle; // only needed for spotlights
	float exponent; // cosine exponent for how light tapers off

} spot_l;


// main loop
void main()
{
	if (draw_line == 1)
	{
		color = vec4(line_color,1.0f);
		return;
	}

	if (draw_skybox == 1)
	{
		color = texture(skybox, position);
		return;
	}

	if (draw_sphere == 1)
	{
		color = vec4(1.0f, 0.3f, 0.3f, 1.0f);
		return;
	}
	else if (draw_sphere == 2)
	{
		color = vec4(0.3f, 1.0f, 0.3f, 1.0f);
		return;
	}

	if (normal_mode == 1)
	{
		color = vec4(normal.x * 0.5f + 0.5f, normal.y * 0.5f + 0.5f, normal.z * 0.5f + 0.5f, 1.0f);
		return;
	}


	// find normal in world coordinate
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal_world = normalize(normalMatrix * normal);

	// calculate the location of this fragment in world coordinates
	vec3 vert = vec3(model * vec4(position, 1.0f));

	vec3 surfaceToLight = vec3(1.0f);

	// Find the L vector, named as surfaceToLight
	// directional light mode, L = -d
	if (light_mode == 1)
	{
		surfaceToLight = vec3( -1.0f * dir_l.light_direction.x, -1.0f * dir_l.light_direction.y, -1.0f * dir_l.light_direction.z);
		surfaceToLight = surfaceToLight / length(surfaceToLight);
	}
	// point light mode, L = p - v / (| p - v |)
	else if (light_mode == 2)
	{
		surfaceToLight = (point_l.light_position - vert) / length(point_l.light_position - vert);
	}
	// spot light mode, L = p - v / (| p - v |)
	else if (light_mode == 3)
	{
		surfaceToLight = (spot_l.light_position - vert) / length(spot_l.light_position - vert);
	}

	// find the lightToSurface vector
	vec3 lightToSurface = -1.0f * surfaceToLight;

	
	if (environment_map == 1)
	{
		vec3 incident_eye = normalize(vert - cam_pos);
		vec3 reflected = reflect(incident_eye, normal);

		reflected = vec3( inverse(view) * vec4(reflected, 0.0f) );
		color = texture(skybox, reflected);
		return;
	}

	// find the R vector
	vec3 R_vec = lightToSurface - (2 * dot(lightToSurface, normal_world)) * normal_world;
	R_vec = normalize(R_vec);

	// find the e vector
	vec3 e_vec = cam_pos - vert;
	e_vec = normalize(e_vec);

	// find the c_l, or light intensity
	vec3 c_l = vec3(0.0f);

	// directional light, c_l = c_light
	if (light_mode == 1)
	{
		c_l = dir_l.light_color;
	}
	// point light, c_l = c_light / (k * |p - v|)
	else if (light_mode == 2)
	{
		c_l = point_l.light_color / ( point_l.attenuation * length(point_l.light_position - vert) );
	}
	// spot light, c_l = c_light / (k * |p - v| ^ 2)
	else if (light_mode == 3)
	{
		float lxd = dot(lightToSurface, spot_l.cone_direction);
		if (lxd <= cos( spot_l.cone_angle )) 
		{
			c_l = vec3(0.0f);
		}
		else
		{
			c_l = spot_l.light_color * pow(lxd, spot_l.exponent) / ( spot_l.attenuation * pow(length(spot_l.light_position - vert),2) );
		}
	}
	else
	{
		c_l = vec3(1.0f);
	}

	// calculate the three scalar values
	vec3 c_d = k_d * dot( surfaceToLight, normal_world) ;
	vec3 c_s = k_s * pow( dot(R_vec, e_vec), 256.0f * shininess) ;
	vec3 c_a = k_a;
	vec3 c_total = c_a + c_s + c_d;

	color = vec4( c_l * c_total, 1.0f);

}
