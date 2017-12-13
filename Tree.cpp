#include "Tree.h"



Tree::Tree()
{
}


Tree::~Tree()
{
}

void Tree::setSeed(int in)
{
	seed = in;
	srand(seed);
}

void Tree::createCylinder(glm::vec3 p1, glm::vec3 p2, float thickness)
{
	// direction serves as a normal
	glm::vec3 direction = glm::normalize(p2 - p1);
	int min = direction[0];
	int index = 0;
	for (int i = 1; i < 3; i++)
	{
		if (direction[i] < min)
		{
			index = i;
			min = direction[i];
		}
	}

	glm::vec3 cross_vec(0.0f);
	cross_vec[index] = 1.0f;

	glm::vec3 product1 = glm::cross(direction, cross_vec);
	glm::vec3 product2 = glm::cross(product1, direction);

	int offset = vertices.size();
	vertices.push_back(p1 + product1*thickness / 2.0f); normals.push_back(product1);
	vertices.push_back(p1 + product2*thickness / 2.0f); normals.push_back(product2);
	vertices.push_back(p1 - product1*thickness / 2.0f); normals.push_back(-1.0f*product1);
	vertices.push_back(p1 - product2*thickness / 2.0f); normals.push_back(-1.0f*product2);

	vertices.push_back(p2 + product1*thickness / 2.0f); normals.push_back(product1);
	vertices.push_back(p2 + product2*thickness / 2.0f); normals.push_back(product2);
	vertices.push_back(p2 - product1*thickness / 2.0f); normals.push_back(-1.0f*product1);
	vertices.push_back(p2 - product2*thickness / 2.0f); normals.push_back(-1.0f*product2);

	std::vector<glm::ivec3> temp;
	temp.push_back(glm::ivec3(0, 4, 5));
	temp.push_back(glm::ivec3(5, 1, 0));
	temp.push_back(glm::ivec3(1, 5, 6));
	temp.push_back(glm::ivec3(6, 2, 1));
	temp.push_back(glm::ivec3(2,6,7));
	temp.push_back(glm::ivec3(7,3,2));
	temp.push_back(glm::ivec3(3,7,4));
	temp.push_back(glm::ivec3(4, 0, 3));

	for (int i = 0; i < temp.size(); i++)
	{
		indices.push_back( (temp[i] + offset) );
		colors.push_back( curr_color );
	}
}

void Tree::initArrays()
{
}

void Tree::render(GLuint shaderProgram)
{
}

Branch::Branch(int seed, Branch * parent)
{
}

Branch::~Branch()
{
}
