#pragma once
/*
* author: Yiming Cai
*/
#include "Model.h"
#include "Light.h"
#include "Util.h"
#include "Camera.h"
#include <map>

class LightDisplay
{
private:

	Light * lights;
	Camera * camera;

	Model * pointLight;
	Model * spotLight;

	// map from shader program to uniform light location
	std::map<GLuint, GLuint> light_loc;

	std::vector< std::pair<glm::vec3, int> > positions_and_types;
	std::vector< glm::vec3 > coneDirections;
	std::vector< int > lightStatus;

public:
	LightDisplay(Light * light, Camera * cam);
	~LightDisplay();

	void initShader(GLuint shaderProgram);
	void update(GLuint shaderProgram);
	void render(GLuint shaderProgram);
};

