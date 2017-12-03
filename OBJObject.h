#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <math.h>
#include "Util.h"



extern glm::mat4 P;
extern glm::mat4 V;
extern GLuint Shader_Geometry;
struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class OBJObject
{
private:
	
	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> colors;
	std::vector<float> localSize;
	
	GLuint *mIndices;
	GLfloat *mVertices;
	GLfloat *mNormals;
	GLfloat *mColors;

public:
	GLuint shader;
	GLuint selfShader;
	glm::mat4 toWorld;
	OBJObject(const char* filepath,float pointSize);
	~OBJObject();
	struct Material material;
	int total;
	int doit;
	float angle;
	float pointSize;
	float xTran, yTran, zTran;
	float scalingX, scalingY, scalingZ,biasX,biasY,biasZ,scaling;
	float orbit;
	float orbitX, orbitY, orbitZ;
	glm::mat4 orbitW;
	glm::mat4 transW;
	GLfloat midX, midY, midZ;
	void parse(const char* filepath);
	void translate();
	void draw();
	void update();
	void spin(float deg);
	void rotate();
	void accumulateUpdate();
	GLuint VBO, VAO, EBO, CBO,NBO;
	GLuint uProjection, uModelview,justModel;
};

#endif