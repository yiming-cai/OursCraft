#include "Transform.h"

void Transform::draw(glm::mat4 C)
{
	this->M = C * this->M;
	for (int i = 0; i < list.size(); ++i) {
		list[i]->draw(this->M);
	}
}

bool Transform::removeChild(int id)
{
	for (int i = 0; i < list.size(); ++i) {
		if (list[i]->getId() == id) {
			list.erase(list.begin() + i);
			return true;
		}
	}
	return false;
}

void Transform::update()
{
	//this->M = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.01)) * M;
	for (int i = 0; i < list.size(); ++i) {
		list[i]->update();
	}
}
