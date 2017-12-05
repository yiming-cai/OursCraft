#ifndef _CUBE_H_
#define _CUBE_H_

#include "Geometry.h"


class Cube: public Geometry {
private:
	static int cubeVerticesLen;
	static GLfloat cubeVertices[24];
	static std::vector<std::string> cubeTexturesAddress;
	static std::vector<GLint> cubeTextureId;
	static int cubeColorsLen;

	static int cubeNormalsLen;
	static GLfloat cubeNormals[24];
	static int cubeIndicesLen ;
	static GLuint cubeIndices[36];
	static GLfloat cubeBoundBoxV[72];
	static GLfloat cubeBoundBoxN[18];
	float size;
	GLfloat cubeColors[24];
	GLuint loadTexture(int pos);
public:
	Cube(int id, float size,glm::vec3 color);
	Cube(int id, float size, int textureSeq);
	~Cube();
};


#endif