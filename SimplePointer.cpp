#include "SimplePointer.h"

SimplePointer::SimplePointer(int id,float xpos, float ypos, glm::vec3 color)
{
	this->id = id;
	pointLen = 0;
	for (int i = xpos - 5; i <= xpos + 5; ++i) {
		this->position[3 * pointLen] = i / 500.0f;
		this->position[3 * pointLen + 1] = ypos;
		this->position[3 * pointLen + 2] = 0;
		this->color[3 * pointLen] = color.x;
		this->color[3 * pointLen + 1] = color.y;
		this->color[3 * pointLen + 2] = color.z;
		++pointLen;
	}
	for (int i = ypos - 10; i <= ypos + 10; ++i) {
		this->position[3 * pointLen] = xpos;
		this->position[3 * pointLen + 1] = i / 500.0f;
		this->position[3 * pointLen + 2] = 0;
		this->color[3 * pointLen] = color.x;
		this->color[3 * pointLen + 1] = color.y;
		this->color[3 * pointLen + 2] = color.z;
		++pointLen;
	}
	this->shader = Shader_SimplePointer;
	printf("this shader is %d\n", Shader_SimplePointer);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * pointLen * sizeof(GLfloat), this ->position, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * pointLen * sizeof(GLfloat), this -> color, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

SimplePointer::~SimplePointer()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &CBO);
}

void SimplePointer::draw(glm::mat4 C)
{
	glUseProgram(shader);
	glm::mat4 modelview = glm::mat4(1.0f);
	//glm::vec4 temp = glm::vec4(vertices[6], vertices[7], vertices[8], 1.0f);
	GLuint uProjection = glGetUniformLocation(shader, "projection");
	GLuint uModelview = glGetUniformLocation(shader, "modelview");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointLen * 3);
	glBindVertexArray(0);

}

void SimplePointer::update()
{
}
