#pragma once
#include "Util.h"

#define GLFW_INCLUDE_GLEXT

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <vector>
#include <algorithm>

class BoundBox
{
public:
	BoundBox();
	BoundBox(float x, float y, float z);
	~BoundBox();
	bool collision = false;
	glm::mat4 toWorld;
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview, uCollision;

	void draw(GLuint shaderProgram);
	void update();
	void spin(float deg);
	bool check_collision(BoundBox * other);
	void setVertices(std::vector <glm::vec3> newVertices);
	std::vector<GLfloat> x_list;
	std::vector<GLfloat> y_list;
	std::vector<GLfloat> z_list;

	const GLuint indices[6][6] = {
		// Front face
		{ 0, 1, 2, 2, 3, 0 },
		// Top face
		{ 1, 5, 6, 6, 2, 1 },
		// Back face
		{ 7, 6, 5, 5, 4, 7 },
		// Bottom face
		{ 4, 0, 3, 3, 7, 4 },
		// Left face
		{ 4, 5, 1, 1, 0, 4 },
		// Right face
		{ 3, 2, 6, 6, 7, 3 }
	};

	
	std::vector<glm::vec3> vertices = {
		// "Front" vertices
		{ -1.0, -1.0,  1.0 },{ 1.0, -1.0,  1.0 },{ 1.0,  1.0,  1.0 },{ -1.0,  1.0,  1.0 },
		// "Back" vertices
		{ -1.0, -1.0, -1.0 },{ 1.0, -1.0, -1.0 },{ 1.0,  1.0, -1.0 },{ -1.0,  1.0, -1.0 }
	};
};
