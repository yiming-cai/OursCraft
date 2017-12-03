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

#define CUBE_TEXTURE_NUM 8

#define MOUSEPOS_INIT_VALUE -1999999
#define CAMERA_MOVE_STRIP 0.2
#define GROUND_LEVEL 0

#endif
