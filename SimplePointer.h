#pragma once

#include "Node.h"

class SimplePointer : public Node {
private:
	GLuint shader;
	GLuint VAO, VBO, CBO;
	GLfloat position[300];
	GLfloat color[300];
	int pointLen;
public:
	SimplePointer(int id,float xpos, float ypos, glm::vec3 color);
	~SimplePointer();
	void draw(glm::mat4 C);
	void update();

};
