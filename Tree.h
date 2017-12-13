#pragma once
#include "Util.h"

const static glm::vec3 brown = glm::vec3(0.4f, 0.2f, 0);
const static glm::vec3 green = glm::vec3(0.4f, 0.6f, 0);

class Branch
{
private:
public:

	Branch * parent;
	std::vector<Branch *> branches;

	Branch(int seed, Branch * parent);
	~Branch();
};


class Tree
{
private:

	int seed = 15;
	Branch * root;
	glm::vec3 curr_color;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> indices;
	std::vector<glm::vec3> colors;

	GLuint VAO, VBO, NBO, CBO, EBO;

public:
	Tree();
	~Tree();

	void setSeed(int);
	void createCylinder(glm::vec3 p1, glm::vec3 p2, float thickness);
	void initArrays();
	void render(GLuint shaderProgram);
};

