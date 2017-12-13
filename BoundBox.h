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
	std::vector<float> x_list;
	std::vector<float> y_list;
	std::vector<float> z_list;
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview, uCollision;
	int count_rotate = 0;
	void draw(GLuint shaderProgram);
	void update();
	bool check_collision(BoundBox * other);
	void setVertices(std::vector <glm::vec3> newVertices);


	const GLuint indices[6][6] = {
		{ 0, 1, 2, 2, 3, 0 },
		{ 1, 5, 6, 6, 2, 1 },
		{ 7, 6, 5, 5, 4, 7 },
		{ 4, 0, 3, 3, 7, 4 },
		{ 4, 5, 1, 1, 0, 4 },
		{ 3, 2, 6, 6, 7, 3 }
	};

	
	std::vector<glm::vec3> vertices = {
		// "Front" vertices
		{ -1.0, -1.0,  1.0 },{ 1.0, -1.0,  1.0 },{ 1.0,  1.0,  1.0 },{ -1.0,  1.0,  1.0 },
		// "Back" vertices
		{ -1.0, -1.0, -1.0 },{ 1.0, -1.0, -1.0 },{ 1.0,  1.0, -1.0 },{ -1.0,  1.0, -1.0 }
	};
};
