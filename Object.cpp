#include "Object.h"

glm::vec3 Object::getObjectBiasPosition(int face)
{
	glm::vec3 pos = toWorld * glm::vec4(boundBoxV[0], boundBoxV[1],boundBoxV[2],1.0f);
	glm::vec3 n = toWorld * glm::vec4(boundBoxN[3 * face], boundBoxN[3 * face + 1], boundBoxN[3 * face + 2],0.0f);
	return pos + n;
}

float Object::getCollisionValue(glm::vec3 point, glm::vec3 rayNormal,int* face)
{
	glm::vec3 planePoint;
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;
	glm::vec3 p4;
	glm::vec3 arcPoint;
	glm::vec3 planeNormal;
	float value = 1000000;
	for (int i = 0; i < 6; ++i) {
		planePoint = glm::vec3(toWorld * glm::vec4(boundBoxV[12 * i], boundBoxV[12 * i + 1], boundBoxV[12 * i + 2], 1.0f));
		p1 = planePoint;
		p2 = glm::vec3(toWorld * glm::vec4(boundBoxV[12 * i + 3], boundBoxV[12 * i + 4], boundBoxV[12 * i + 5] , 1.0f));
		p3 = glm::vec3(toWorld * glm::vec4(boundBoxV[12 * i + 6], boundBoxV[12 * i + 7], boundBoxV[12 * i + 8], 1.0f));
		p4 = glm::vec3(toWorld * glm::vec4(boundBoxV[12 * i + 9], boundBoxV[12 * i + 10], boundBoxV[12 * i + 11] , 1.0f));
		planeNormal = glm::vec3(boundBoxN[3 * i], boundBoxN[3 * i + 1], boundBoxN[3 * i + 2]);
		planeNormal = glm::normalize(planeNormal);
		rayNormal = glm::normalize(rayNormal);
		float t = (glm::dot(planeNormal, planePoint) - glm::dot(planeNormal, point)) / (glm::dot(rayNormal, planeNormal));
		if (t <= 0) continue;
		arcPoint = point + t * rayNormal;
		if (glm::dot(arcPoint - p1, p2 - p1) >= 0 && glm::dot(arcPoint - p2, p3 - p2) >= 0 &&
			glm::dot(arcPoint - p3, p4 - p3) >= 0 && glm::dot(arcPoint - p4, p1 - p4) >= 0) {
			float k = glm::dot(arcPoint - point, arcPoint - point);
			if (value >= k) {
				value = k;
				*face = i;
			}
		}
	}
	if (value >= 999999) return -1;
	return value;
	
}
