#include "Translate.h"

Translate::Translate(int id,float x, float y, float z)
{
	this->id = id;
	this->M = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

Translate::Translate(int id,glm::vec3 pos)
{
	this->id = id;
	this->M = glm::translate(glm::mat4(1.0f), pos);

}
