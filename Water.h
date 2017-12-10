#pragma once
#include "Geometry.h"
#include <time.h>

class Water : public Geometry {
private:
	static std::vector<std::string> waterTexturesAddress;
	glm::vec3 *buffer;
	GLfloat *buffer2,*buffer3;
	float size;
	glm::vec3 waterColor;
	float total, water_v, water_u,water_t;
	float f1, f2, k1, k2, k3;
	GLuint loadTexture(int pos);
public:
	Water(int id, int size, float total, float v, float u,float t, glm::vec3 color);
	Water(int id, int size, float total, float v, float u, float t, int tex);
	~Water();
	void waterUpdate();
	void draw(glm::mat4 C);
};