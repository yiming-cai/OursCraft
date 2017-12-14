#pragma once
#include "Util.h"

const static glm::vec3 brown = glm::vec3(83.0f/255.0f, 53.0f / 255.0f, 10.0f / 255.0f) * 0.6f;
const static glm::vec3 green = glm::vec3(0.4f, 0.6f, 0);

extern glm::mat4 P;
extern glm::mat4 V;

class Branch
{
private:
public:

	Branch * parent;
	glm::vec3 start_point;
	glm::vec3 end_point;
	float start_size;
	float end_size;
	std::vector<Branch *> branches;

	Branch(int, float, float, float, float, Branch * parent, glm::vec3 origin, glm::vec3 direction, std::vector<Branch*>&);
	~Branch();
};


class Tree
{
private:

	int seed = 15;
	Branch * root;
	glm::vec3 curr_color = brown;
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> indices;
	std::vector<glm::vec3> colors;

	GLuint VAO, VBO, NBO, CBO, EBO, TBO;
	const static int vertexLoc=0, normalLoc=1, colorLoc=2, textureLoc=3;

	// some variables that define the properties of the tree
	int iterations;
	float angleleft;
	float angleright;
	float initial_length;
	float trunk_thickness;
	int numBranches;
	std::vector <Branch *> allBranches;

public:
	Tree(int Seed);
	~Tree();

	void setSeed(int);
	void createCylinder(glm::vec3 p1, glm::vec3 p2, float thicknessstart, float thickness_end);
	void bindBuffers();
	void render(GLuint shaderProgram);
	void setPosition(float x, float z) { modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)); }
};

