#ifndef _CUBE_H_
#define _CUBE_H_

#include "Geometry.h"

// cube info for storing the information that actually makes a cube unique
struct CubeInfo {
	glm::vec3 position;
	glm::vec3 color;
	float size;
	int id;
	int textureId;
};

class Cube: public Geometry {
private:
	static int cubeVerticesLen;
	static GLfloat cubeVertices[108];
	static std::vector<std::string> cubeTexturesAddress;
	static std::vector<GLint> cubeTextureId;
	static int cubeColorsLen;

	static int cubeNormalsLen;
	static GLfloat cubeNormals[108];
	static GLfloat cubeBoundBoxV[72];
	static GLfloat cubeBoundBoxN[18];
	float size;
	GLfloat cubeColors[108];
	GLuint loadTexture(int pos);

	CubeInfo info;

public:
	Cube(int id, float size,glm::vec3 color);
	Cube(int id, float size, int textureSeq);
	~Cube();

	void setPosition(float x, float y, float z) { Geometry::setPosition(x, y, z); info.position = glm::vec3(x, y, z); }
	CubeInfo getCubeInfo() { return info; }
};


#endif
