#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Node.h"
#include "Geometry.h"
#include <math.h>

class Transform : public Node{
public:
	// Geometry variables
	glm::mat4 M;
	std::vector<Node *> children;

	// member functions
	void addChild(Node * child);
	void removeChild(Node * child);
	void draw(GLuint shaderProgram, glm::mat4 C);
	void update();

	Transform();
	~Transform();
};

class Robot : public Transform{
public:

	Transform *t_body, *t_left_leg, *t_right_leg, *t_left_arm, *t_right_arm, *t_left_eyeball, *t_right_eyeball, *t_left_antenna, *t_right_antenna, *t_head;

	bool pos_angle = true, pos_leg = true;
	float dist_travelled = 0.0f;
	float hand_angle = 0.0f;
	float leg_pos = 0.0f;
	void update();
	Robot();
	~Robot();
};

class ArmyOfRobot : public Transform {
public:

	class OBJObject * sphere;
	float radius = 2.45f;
	Robot * robot;
	int grid_size = 200;
	int army_size = grid_size * grid_size;
	std::vector<bool> inside;

	void update();
	ArmyOfRobot();
	~ArmyOfRobot();
	void drawBoundingSphere(GLuint shaderProgram, glm::mat4 C);
	void cull(std::vector<std::pair< glm::vec3, glm::vec3 > > listOfPlanes, glm::mat4 C, glm::mat4 P, glm::mat4 V);
	void draw(GLuint shaderProgram, glm::mat4 C);
};

class World : public Transform {
public:	

	World();
	~World();
	void update();
	//void drawBoundingSphere(GLuint shaderProgram, glm::mat4 C);
	//void cull(std::vector<std::pair< glm::vec3, glm::vec3 > > listOfPlanes, glm::mat4 C, glm::mat4 P, glm::mat4 V);
	bool changeCPPosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V);
	void continueChangePosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V);
	void reset();
	void changeCamera(glm::mat4 & V, glm::vec3 &, glm::vec3 &, glm::vec3 &);
	void turnFrictionOn();
};

class ConnectedCurves : public Transform {
public:

	class OBJObject * sphere;
	const int numCurves = 8;
	int prev_whichVector = -1;
	int prev_currIndex = -1;

	float prev_pos = 0.0f;
	float curr_pos = 0.0f;
	glm::vec3 sphere_pos;
	const float MAX_POS = 8.0f;
	float initial_speed = 1.0f;
	float curr_speed = initial_speed;
	float total_energy = 0.0f;
	float starting_energy = 0.0f;
	bool flipped = false;
	bool friction = false;
	float delta_t = 0.05f;
	float y_offset = 0.0f;
	std::vector<float> total_lengths;

	std::vector<glm::vec3> controlPoints = std::vector<glm::vec3>(numCurves+1, glm::vec3(0.0f));
	std::vector<glm::vec3> thirdPoints = std::vector<glm::vec3>(numCurves, glm::vec3(0.0f));
	std::vector<glm::vec3> secondPoints = std::vector<glm::vec3>(numCurves, glm::vec3(0.0f));
	std::vector<class LineSegment *> controlLines;
	std::vector<class BenzierCurve *> curves;

	ConnectedCurves();
	~ConnectedCurves();

	void update();
	float findHighestPosition();
	float findLowestPoint();
	float findY(float pos);

	bool changeCPPosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V, glm::mat4 M);
	void continueChangePosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V, glm::mat4 M);
	void draw(GLuint shaderProgram, glm::mat4 C);
	void printVertices();
	glm::vec3 findPoint(float pos);
	void reset();
	void changeCamera(glm::mat4 & V, glm::vec3 & cam_pos, glm::vec3 & cam_look_at, glm::vec3 &cam_up);
	void turnFrictionOn();
	float normalize(float);
	float findNewPos(float, std::vector<float>, std::vector<float>);
};

class BenzierCurve : public Transform {
public:

	const glm::mat4 B_bez = { { -1.0f,3.0f,-3.0f,1.0f },{ 3.0f,-6.0f,3.0f,0.0f },{ -3.0f,3.0f,0.0f,0.0f },{ 1.0f,0.0f,0.0f,0.0f } };

	glm::vec3 cp1, cp2, cp3, cp4;
	
	bool accelerator = false;
	float total_length;

	BenzierCurve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
	~BenzierCurve();

	void update();
	void changeControlPoints(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);

	glm::vec3 findPoint(float t);
	glm::vec3 findPointByLength(float l);
	void draw(GLuint shaderProgram, glm::mat4 C);
};



#endif