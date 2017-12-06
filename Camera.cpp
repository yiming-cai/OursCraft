#include "Camera.h"

Camera::Camera(int id, glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up)
{
	this->camera_pos = pos;
	this->camera_lookAt = lookAt;
	this->camera_up = up;
	this->id = id;
	this->camera_lookAt = glm::normalize(camera_lookAt - camera_pos) + camera_pos;
}

Camera::~Camera()
{
}

glm::mat4 Camera::getCameraV()
{
	return glm::lookAt(camera_pos, camera_lookAt, camera_up);
}

void Camera::draw(glm::mat4 C)
{
}

void Camera::update()
{
}


void Camera::cameraMove(int dir) { // 
	glm::vec3 rz = glm::normalize(camera_lookAt - camera_pos);
	glm::vec3 ry = glm::vec3(0, 1, 0) * 0.5f;
	glm::vec3 rx = glm::normalize(glm::cross(rz, ry)) * 0.5f;
	rz = glm::normalize(glm::cross(ry, rx)) * 0.5f;
	float strip = CAMERA_MOVE_STRIP;
	rx *= strip;
	ry *= strip;
	rz *= strip;
	switch (dir)
	{
	case 0:camera_pos += rz; camera_lookAt += rz; break; // forward
	case 1:camera_pos -= rz; camera_lookAt -= rz; break; // backward
	case 2:camera_pos += rx; camera_lookAt += rx; break; // right
	case 3:camera_pos -= rx; camera_lookAt -= rx; break; // left
	case 4:camera_pos += ry; camera_lookAt += ry; break; // up
	case 5:camera_pos -= ry; camera_lookAt -= ry; break; // down
	default:
		break;
	}
	if (camera_pos.y < GROUND_LEVEL + 0.5) {
		camera_lookAt.y += GROUND_LEVEL + 0.5 - camera_pos.y;
		camera_pos.y = GROUND_LEVEL + 0.5;
	}
}
void Camera::cameraRotate(glm::vec2 delta)
{
	glm::vec3 rz = glm::normalize(camera_lookAt - camera_pos);
	glm::vec3 ry = camera_up;
	glm::vec3 rx = glm::normalize(glm::cross(rz, ry));
	glm::vec3 re = glm::vec3(0, 0, 0);
	//printf("delta %f %f\n", delta.x, delta.y);
	re += (delta.x * rx);
	re += (delta.y * ry);
	float xx = delta.x * delta.x * glm::dot(rx, rx);
	float yy = delta.y * delta.y * glm::dot(ry,ry);
	float zz = 1.0f - xx - yy;
	if (zz < 0) zz = 0;
	
	zz = sqrt(zz);
//	printf("xyz: %f %f %f\n", xx, yy, zz);
	re += (zz * rz);
	re = glm::normalize(re);
	camera_lookAt = camera_pos + re;
	//printf("LookAt %f %f %f\n", re.x, re.y, re.z);
	glm::vec3 groundP = glm::vec3(0, -1, 0);
	camera_up = glm::normalize(glm::cross(glm::cross(groundP, re), re));
}
