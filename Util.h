#pragma once

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
#define DISPLAYLIGHT_VERTEX_SHADER_PATH "../displayLightShader.vert"
#define DISPLAYLIGHT_FRAGMENT_SHADER_PATH "../displayLightShader.frag"
#define SIMPLE_POINTER_VERTEX_SHADER_PATH "../simplePointerShader.vert"
#define SIMPLE_POINTER_FRAGMENT_SHADER_PATH "../simplePointerShader.frag"
#define BOUNDBOX_VERTEX_SHADER_PATH "../boundboxShader.vert"
#define BOUNDBOX_FRAGMENT_SHADER_PATH "../boundboxShader.frag"
#define WATER_VERTEX_SHADER_PATH "../waterShader.vert"
#define WATER_FRAGMENT_SHADER_PATH "../waterShader.frag"
#define CUBE_TEXTURE_NUM 8

#define WATER_HEIGHT 1
#define MOUSEPOS_INIT_VALUE -1999999
#define CAMERA_MOVE_STRIP 0.4
#define GROUND_LEVEL 0

#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720
