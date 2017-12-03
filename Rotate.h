#pragma once
#include "Transform.h"

class Rotate : public Transform {
public:
	Rotate(int id, float angle, glm::vec3 axis);
};