#pragma once
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <errno.h>

class Node
{
public:
	virtual void draw(GLuint shaderProgram, glm::mat4 C) = 0;
	virtual void update() = 0;
};

