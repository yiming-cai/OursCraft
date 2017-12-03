#pragma once
#include "Transform.h"

class Translate :public Transform {
public:
	Translate(int id,float x, float y, float z);
	Translate(int id,glm::vec3 pos);
};