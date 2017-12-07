#ifndef _UTIL_H_
#define _UTIL_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

#define OBJECT_CUBE 101
#define OBJECT_SPHERE 102
#define OBJECT_CONE 103
#define OBJECT_DOMINO 104

#define OBJECT_MODEL 120

#define OBJECT_SKYBOX 200

#define GEOMETRY_VERTEX_SHADER_PATH "../geometryShader.vert"
#define GEOMETRY_FRAGMENT_SHADER_PATH "../geometryShader.frag"
#define SKYBOX_VERTEX_SHADER_PATH "../skyboxShader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../skyboxShader.frag"
#define COORDINATE_VERTEX_SHADER_PATH "../coordinateShader.vert"
#define COORDINATE_FRAGMENT_SHADER_PATH "../coordinateShader.frag"
#define MODEL_VERTEX_SHADER_PATH "../modelShader.vert"
#define MODEL_FRAGMENT_SHADER_PATH "../modelShader.frag"
#define SIMPLE_POINTER_VERTEX_SHADER_PATH "../simplePointerShader.vert"
#define SIMPLE_POINTER_FRAGMENT_SHADER_PATH "../simplePointerShader.frag"
#define BOUNDBOX_VERTEX_SHADER_PATH "../boundboxShader.vert"
#define BOUNDBOX_FRAGMENT_SHADER_PATH "../boundboxShader.frag"
#define CUBE_TEXTURE_NUM 8

#define MOUSEPOS_INIT_VALUE -1999999
#define CAMERA_MOVE_STRIP 0.1
#define GROUND_LEVEL 0

struct LightParameters
{
	glm::vec4 position; // also used as direction for directional light
	glm::vec4 intensities; // a.k.a the color of the light
	glm::vec4 coneDirection; // only needed for spotlights

	float attenuation; // only needed for point and spotlights
	float ambientCoefficient; // how strong the light ambience should be... 0 if there's no ambience (background reflection) at all
	float coneAngle; // only needed for spotlights
	float exponent; // cosine exponent for how light tapers off
	int type; // specify the type of the light (directional, spotlight, point light)
	int attenuationType; // specify the type of attenuation to use
	int status;			// 0 for turning off the light, 1 for turning on the light
	int PADDING;
};

struct MyMaterial {

	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emissive[4];
	float shininess;
	int texCount;
};

// Information to render each assimp node
struct MyMesh {

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint nbo;
	GLuint texIndex;
	GLuint uniformBlockIndex;
	int numFaces;
};

#endif
