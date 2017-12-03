#pragma once
#include "Node.h"

class Camera :public Node {
private:
	GLuint shader;
public: 
	glm::vec3 camera_pos;
	glm::vec3 camera_lookAt;
	glm::vec3 camera_up;
	Camera(int id,glm::vec3 pos,glm::vec3 lookAt,glm::vec3 up);
	~Camera();
	glm::mat4 getCameraV();
	void draw(glm::mat4 C);
	void update();
	void cameraRotate(glm::vec2 delta);
	void cameraMove(int dir);
};

