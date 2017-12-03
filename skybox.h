#pragma once
#include "Object.h"
#include "stb_image.h"

class Skybox : public Object {
private:
	static GLfloat skyboxVertices[108];

	std::vector<std::string> *faces;
	GLuint shader;
	float distance;
	GLuint texture;
	GLuint loadCubemap();
public:
	Skybox(int id, float distance, std::vector<std::string> *faces);
	~Skybox();
	void draw(glm::mat4 C);
	void update();
	void setSelected() {

	}
	
};