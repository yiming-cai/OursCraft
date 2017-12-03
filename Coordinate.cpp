#include "Coordinate.h"

Coordinate::Coordinate(int id, int size)
{
	this->id = id;
	this->size = size;
	this->shader = Shader_Coordinate;
	this->toWorld = glm::mat4(1.0f);
	len = (2 * size + 1) * 2 * 3 * 2;
	lineV = new GLfloat[len];
	int pos = 0;
	for (int i = -size; i <= size; ++i) {
		lineV[pos++] = i;
		lineV[pos++] = GROUND_LEVEL;
		lineV[pos++] = size;
		lineV[pos++] = i;
		lineV[pos++] = GROUND_LEVEL;
		lineV[pos++] = -size;
	}
	for (int i = -size; i <= size; ++i) {
		lineV[pos++] = -size;
		lineV[pos++] = GROUND_LEVEL;
		lineV[pos++] = i;
		lineV[pos++] = size;
		lineV[pos++] = GROUND_LEVEL;
		lineV[pos++] = i;
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLfloat), lineV, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	printf("Coordinate Load!\n");
}

Coordinate::~Coordinate()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Coordinate::draw(glm::mat4 C)
{
	//printf("Coordinate draw!\n");
	glUseProgram(shader);
	glm::mat4 modelview = V * C * toWorld;
	glm::vec3 temp = glm::vec3(lineV[3], lineV[4], lineV[5]);
	uProjection = glGetUniformLocation(shader, "projection");
	uModelview = glGetUniformLocation(shader, "modelview");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glBindVertexArray(VAO);

	glDrawArrays(GL_LINES, 0, (2 * size + 1) * 4);

	glBindVertexArray(0);
}

void Coordinate::update()
{
}
