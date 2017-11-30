#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <errno.h>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <limits>
#include <math.h>

#include "Window.h"

class OBJObject
{
private:

	// variables for the shader program
	GLuint VBO, VAO, EBO, NBO;
	GLuint uProjection, uModel, uView;
	GLuint normal_mode;

	// constant for conversion between degrees and radians
	const float degree = 1.0f / 180.0f * glm::pi<float>();

	// helper functions
	void parseVertex(std::string, float &, float &, float &);
	void parseFace(std::string, unsigned int &, unsigned int &, unsigned int &);
	void normalize_vertices();
	glm::vec3 trackBallMapping(double xpos, double ypos);

public:
	
	// values stored by the parser
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::uvec3> faces;

	// the object->world matrix
	glm::mat4 toWorld;

	// current point size
	float point_size = 1.0f;

	int i_normal_mode;

	// variables used to define the material property and color
	float shininess;
	glm::vec3 k_d; // diffuse reflectance coefficient
	glm::vec3 k_s; // spectacular reflectance coefficient
	glm::vec3 k_a; // ambient reflectance coefficient

	// these constants are used for translation function
	const static int UP = 1, DOWN = -1;
	const static int LEFT = -2, RIGHT = 2;
	const static int OUT = 3, IN = -3;
	const static int RESET_ORIENTATION = 5, RESET_POSITION = -5;
	const static int SCALE_UP = 6, SCALE_DOWN = -6;
	const static int ORBIT_COUNTER_CW = 7, ORBIT_CW = -7;

	// transformation and update functions
	OBJObject(const char* filepath);
	~OBJObject();
	void parse(const char* filepath);
	void draw(GLuint);
	void spin(float);
	void update();
	void increase_point_size();
	void decrease_point_size();
	void setPointSize();
	void translate(int);
	void orbit(int);
	void reset(int);
	void scale(int);

	// Project 2 new functions
	void translate(double, double);
	void setPosition(glm::vec3);
	void setSize(float);
	void rotate(double p_xPos, double p_yPos, double xPos, double yPos);
	void switchRenderMode();
	glm::vec3 getPosition();
};

#endif