#include "BoundBox.h"
#include "Window.h"

extern glm::mat4 P;
extern glm::mat4 V;

BoundBox::BoundBox()
{
	collision = false;
	toWorld = glm::mat4(1.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(GLvoid*)0); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	update();
}

BoundBox::BoundBox(float x, float y, float z)
{
	collision = false;
	toWorld = glm::mat4(1.0f);

	for (int i = 0; i < 8; i++) {
		vertices[i][0] *= x;
		vertices[i][1] *= y;
		vertices[i][2] *= z;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(GLvoid*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	update();
}

BoundBox::~BoundBox()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void BoundBox::draw(GLuint shaderProgram)
{
	glDisable(GL_CULL_FACE);
	glm::mat4 modelview = V * toWorld;
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uCollision = glGetUniformLocation(shaderProgram, "collision_color");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	if (collision == true) {
		glUniform3f(uCollision, 1.0f, 0.0f, 0.0f);
	}
	else {
		glUniform3f(uCollision, 0.0f, 1.0f, 0.0f);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
}

bool BoundBox::check_collision(BoundBox * other) {
	std::sort(this->x_list.begin(), this->x_list.end());
	std::sort(this->y_list.begin(), this->y_list.end());
	std::sort(this->z_list.begin(), this->z_list.end());

	std::sort(other->x_list.begin(), other->x_list.end());
	std::sort(other->y_list.begin(), other->y_list.end());
	std::sort(other->z_list.begin(), other->z_list.end());
	
	GLfloat source_x_min = x_list[0];
	GLfloat source_x_max = x_list[7];
	GLfloat source_y_min = y_list[0];
	GLfloat source_y_max = y_list[7];
	GLfloat source_z_min = z_list[0];
	GLfloat source_z_max = z_list[7];

	GLfloat target_x_min = other->x_list[0];
	GLfloat target_x_max = other->x_list[7];
	GLfloat target_y_min = other->y_list[0];
	GLfloat target_y_max = other->y_list[7];
	GLfloat target_z_min = other->z_list[0];
	GLfloat target_z_max = other->z_list[7];

	bool x_collides = false;
	bool y_collides = false;
	bool z_collides = false;

	this->collision = false;
	other->collision = false;

	// Check x-axis overlaps
	if (source_x_min >= target_x_min && source_x_min <= target_x_max
		|| source_x_max >= target_x_min && source_x_max <= target_x_max) {
		x_collides = true;
	}

	// Check y-axis overlaps
	if (source_y_min >= target_y_min && source_y_min <= target_y_max
		|| source_y_max >= target_y_min && source_y_max <= target_y_max) {
		y_collides = true;
	}

	// Check z-axis overlaps
	if (source_z_min >= target_z_min && source_z_min <= target_z_max
		|| source_z_max >= target_z_min && source_z_max <= target_z_max) {
		z_collides = true;
	}

	// Checks for collision
	if (x_collides && y_collides && z_collides) {
		this->collision = true;
		other->collision = true;
		return true;
	}
	else {
		return false;
	}
}

void BoundBox::setVertices(std::vector<glm::vec3> newVertices)
{
	vertices = newVertices; 
	update();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0][0]);
}

void BoundBox::update()
{
	x_list.clear();
	y_list.clear();
	z_list.clear();
	for (int i = 0; i < 8; i++) {
		glm::vec4 new_point = glm::vec4(vertices[i][0], vertices[i][1], vertices[i][2], 1.0f);
		new_point = this->toWorld * new_point;
		x_list.push_back(new_point[0]);
		y_list.push_back(new_point[1]);
		z_list.push_back(new_point[2]);
	}
	std::sort(this->x_list.begin(), this->x_list.end());
	std::sort(this->y_list.begin(), this->y_list.end());
	std::sort(this->z_list.begin(), this->z_list.end());


}


