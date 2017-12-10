#pragma once

#include "Util.h"
#include "Camera.h"

class Fog {
private:
	glm::vec3 color;
	std::vector<GLuint> *shaderList;
	float rate;
	float start, end,tempend;
	Camera *camera;
public:
	Fog(glm::vec3 color, Camera *camera, float start,float end,float rate);
	void setCamera(Camera *camera);
	void setColor(glm::vec3 color);
	void addShader(GLuint shader);
	void fogUpdate(int available);
	void setRate(float rate);

};