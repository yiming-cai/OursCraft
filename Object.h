#ifndef _OBJECT_H_
#define _OBJECT_H_


#include "Node.h"


class Object : public Node {
protected:
	GLfloat *vertices,*normals,*colors;
	GLuint *indices;
	int verticesLen, colorsLen, normalsLen,indicesLen;
	glm::mat4 toWorld;
	int nameID;
	GLuint VAO, VBO, NBO, CBO, EBO;
	GLuint uProjection, uModelview;
	GLfloat *boundBoxV, *boundBoxN;
public:
	virtual void draw(glm::mat4 C) = 0;
	virtual void update() = 0;
	virtual void setSelected() = 0;
	void setPosition(float x, float y, float z) {
		toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	}
	glm::vec3 getObjectBiasPosition(int face);
	float getCollisionValue(glm::vec3 point, glm::vec3 rayNormal,int *face);
	

};

#endif