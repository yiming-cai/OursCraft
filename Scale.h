#pragma once
#include "Transform.h"

class Scale :public Transform {
public:
	Scale(int id, float x, float y, float z);
	Scale(int id, glm::vec3 size);
};