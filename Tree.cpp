#include "Tree.h"



Tree::Tree(int seed)
{
	srand(seed);
	iterations = rand() % 10 + 5;
	angleleft = float(rand() % 50 + 30) * glm::pi<float>() / 180.0f;
	angleright = -1.0f * float(rand() % 50 + 30) * glm::pi<float>() / 180.0f;
	initial_length = float(rand() % 50 + 20)/10.0f;
	trunk_thickness = float(rand() % 5 + 7)/10.0f;

	root = new Branch(iterations, angleleft, angleright, initial_length, trunk_thickness, root, glm::vec3(0), glm::vec3(0,1,0), allBranches);
	for (int i = 0; i < allBranches.size(); i++)
	{
		createCylinder(allBranches[i]->start_point, allBranches[i]->end_point, allBranches[i]->start_size, allBranches[i]->end_size);
	}
	bindBuffers();
}


Tree::~Tree()
{
}

void Tree::setSeed(int in)
{
	seed = in;
	srand(seed);
}

void Tree::createCylinder(glm::vec3 p1, glm::vec3 p2, float thickness_start, float thickness_end)
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

	glm::vec3 product1 = glm::normalize(glm::cross(direction, cross_vec));
	glm::vec3 product2 = glm::normalize(glm::cross(product1, direction));

	int offset = vertices.size();
	vertices.push_back(p1 + product1*thickness_start / 2.0f); normals.push_back(product1);
	vertices.push_back(p1 + product2*thickness_start / 2.0f); normals.push_back(product2);
	vertices.push_back(p1 - product1*thickness_start / 2.0f); normals.push_back(-1.0f*product1);
	vertices.push_back(p1 - product2*thickness_start / 2.0f); normals.push_back(-1.0f*product2);

	vertices.push_back(p2 + product1*thickness_end / 2.0f); normals.push_back(product1);
	vertices.push_back(p2 + product2*thickness_end / 2.0f); normals.push_back(product2);
	vertices.push_back(p2 - product1*thickness_end / 2.0f); normals.push_back(-1.0f*product1);
	vertices.push_back(p2 - product2*thickness_end / 2.0f); normals.push_back(-1.0f*product2);

	for (int i = 0; i < 8; i++)
	{
		colors.push_back(curr_color);
	}

	std::vector<glm::ivec3> temp;
	temp.push_back(glm::ivec3(0, 4, 5));
	temp.push_back(glm::ivec3(5, 1, 0));
	temp.push_back(glm::ivec3(1, 5, 6));
	temp.push_back(glm::ivec3(6, 2, 1));
	temp.push_back(glm::ivec3(2,6,7));
	temp.push_back(glm::ivec3(7,3,2));
	temp.push_back(glm::ivec3(3,7,4));
	temp.push_back(glm::ivec3(4,0,3));
	temp.push_back(glm::ivec3(0,1,2));
	temp.push_back(glm::ivec3(2, 3, 0));
	temp.push_back(glm::ivec3(4,7,6));
	temp.push_back(glm::ivec3(6,5,4));

	for (int i = 0; i < temp.size(); i++)
	{
		indices.push_back( ( glm::vec3(temp[i].x + offset, temp[i].y+offset, temp[i].z+offset)) );
	}

}

void Tree::bindBuffers()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size() * 3, &(indices[0]),GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertices.size(), &(vertices[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);

	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * normals.size(), &(normals[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(normalLoc);
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);

	glGenBuffers(1, &CBO);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * colors.size(), &(colors[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, 0, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Tree::render(GLuint shaderProgram)
{
	//glUseProgram(shaderProgram);

	GLuint uProjection = glGetUniformLocation(shaderProgram, "projection");
	GLuint uView = glGetUniformLocation(shaderProgram, "view");
	GLuint uModel = glGetUniformLocation(shaderProgram, "model");
	GLuint uCam = glGetUniformLocation(shaderProgram, "cam_pos");
	glm::vec3 cam_pos = glm::vec3(V[3][0], V[3][1], V[3][2]);

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniform3fv(uCam, 1, &(cam_pos[0]));

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers

	glBindVertexArray(0);
}

Branch::Branch(int numIterations, float angleLeft, float angleRight, float length, float thickness, Branch * parent, glm::vec3 origin, glm::vec3 direction, std::vector<Branch*>& all)
{
	start_point = origin;
	end_point = origin + direction * length;
	start_size = thickness;
	end_size = thickness/(float(rand()%150)/100.0f + 1.5f);
	all.push_back(this);
	if (numIterations == 0)
		return;

	float next_thickness = thickness / 2.0f;
	glm::vec3 axis = glm::vec3(float(rand()%100), 0, float(rand()%100));
	axis = glm::normalize(axis);
	glm::vec3 leftDir = glm::normalize( glm::vec3( rotate(glm::mat4(1.0f), angleLeft, axis) * glm::vec4(direction,1.0f) ) );
	glm::vec3 rightDir = glm::normalize( glm::vec3( rotate(glm::mat4(1.0f), angleRight, axis) * glm::vec4(direction, 1.0f)) );
	branches.push_back( new Branch(numIterations-1, angleLeft, angleRight,length/2.0f, end_size,this,this->end_point,leftDir,all ) );
	branches.push_back( new Branch( numIterations - 1, angleLeft, angleRight, length / 2.0f, end_size, this, this->end_point, rightDir,all));
}

Branch::~Branch()
{
}
