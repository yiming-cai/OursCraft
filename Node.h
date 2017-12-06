#ifndef _NODE_H_
#define _NODE_H_

#include "Util.h"
#include "stb_image.h"


extern glm::mat4 P;
extern glm::mat4 V;
extern GLuint Shader_Geometry;
extern GLuint Shader_Skybox;
extern GLuint Shader_Coordinate;
extern GLuint Shader_Model;
extern GLuint Shader_SimplePointer;

class Node {
protected:
	int id;
public:
	virtual void draw(glm::mat4 C) = 0;
	virtual void update() = 0;
	int getId() { return this->id; }
};


#endif