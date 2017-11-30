#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Node.h"
#include "OBJObject.h"

class Geometry : public Node {
public:
	// Geometry variables
	glm::mat4 model;
	class OBJObject * obj = 0;

	// member functions
	void draw(GLuint shaderProgram, glm::mat4 C);
	void update();
	void initObj(const char * filepath);

	Geometry();
	~Geometry();
};

class Antenna : public Geometry {
public:
	Antenna();
	~Antenna();
};

class Head : public Geometry {
public:
	Head();
	~Head();
};

class Body: public Geometry {
public:
	Body();
	~Body();
};

class Leg : public Geometry {
public:
	Leg ();
	~Leg();
};

class Arm : public Geometry {
public:
	Arm ();
	~Arm();
};


class Eyeball : public Geometry {
public:
	Eyeball();
	~Eyeball();
};

class Bunny : public Geometry {
public:
	Bunny();
	~Bunny();
};

class LineSegment : public Geometry {
public:

	glm::vec3 p1, p2;
	float angle;
	float pos;

	LineSegment(glm::vec3 p_p1, glm::vec3 p_p2);
	~LineSegment();

	void changePoints(glm::vec3 p_p1, glm::vec3 p_p2);
	void draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 color);
	float getLength();
};

#endif