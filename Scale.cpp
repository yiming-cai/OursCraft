#include "Scale.h"

Scale::Scale(int id, float x, float y, float z)
{
	this->id = id;
	this->M = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));

}

Scale::Scale(int id, glm::vec3 size)
{
	this->id = id;
	this->M = glm::scale(glm::mat4(1.0f), size);
}
