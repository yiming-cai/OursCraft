#pragma once

#include "Node.h"

class Coordinate :public Node {
private:
	GLuint VAO, VBO;
	GLfloat *lineV;
	glm::mat4 toWorld;
	GLuint shader;
	int size;
	int len;
	GLuint uProjection, uModelview;
	int nameID;
public:
	Coordinate(int id, int size);
	~Coordinate();
	void draw(glm::mat4 C);
	void update();

};
