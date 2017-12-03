#include "Rotate.h"

Rotate::Rotate(int id, float angle, glm::vec3 axis)
{
	this->id = id;
	this->M = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis);
}
