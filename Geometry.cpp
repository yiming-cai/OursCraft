#include "Geometry.h"

void Geometry::draw(GLuint shaderProgram, glm::mat4 C)
{
	model = C;
	
	// draw the stuffs here
	if (obj != 0)
	{
		obj->toWorld = model;
		obj->draw(shaderProgram);
	}
}

void Geometry::update()
{

}

void Geometry::initObj(const char * filepath)
{
	obj = new OBJObject(filepath);
}

Geometry::Geometry()
{

}

Geometry::~Geometry()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

Antenna::Antenna()
{
	initObj("../robot_assets/antenna.obj");
	obj->k_a = glm::vec3(0.2f, 0.4f, 0.2f);	// ambient reflection 
	obj->k_d = glm::vec3(0.1f, 0.35f, 0.1f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.45f, 0.55f, 0.45f);		// specular reflection
	obj->shininess = .75f;
}

Antenna::~Antenna()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

Head::Head()
{
	initObj("../robot_assets/head.obj");
	obj->k_a = glm::vec3(0.2f, 0.4f, 0.2f);	// ambient reflection 
	obj->k_d = glm::vec3(0.1f, 0.35f, 0.1f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.45f, 0.55f, 0.45f);		// specular reflection
	obj->shininess = .75f;
}

Head::~Head()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

Body::Body()
{
	initObj("../robot_assets/body.obj");
	obj->k_a = glm::vec3(0.1745f, 0.01175f, 0.01175f);	// ambient reflection 
	obj->k_d = glm::vec3(0.61424f, 0.04136f, 0.04136f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.727811f, 0.626959f, 0.626959f);		// specular reflection
	obj->shininess = 0.99f;
}

Body::~Body()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

Leg::Leg()
{
	initObj("../robot_assets/limb.obj");	
	obj->k_a = glm::vec3(0.2f, 0.1f, 0.3f);	// ambient reflection 
	obj->k_d = glm::vec3(0.2f, 0.2f, 0.4f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.45f, 0.2f, 0.65f);		// specular reflection
	obj->shininess = .25f;
}

Leg::~Leg()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

Arm::Arm()
{
	initObj("../robot_assets/limb.obj");
	obj->k_a = glm::vec3(0.2f, 0.1f, 0.3f);	// ambient reflection 
	obj->k_d = glm::vec3(0.2f, 0.2f, 0.4f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.45f, 0.2f, 0.65f);		// specular reflection
	obj->shininess = .25f;
}

Arm::~Arm()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

Eyeball::Eyeball()
{
	initObj("../robot_assets/eyeball.obj");
	obj->k_a = glm::vec3(0.0f, 0.0f, 0.0f);	// ambient reflection 
	obj->k_d = glm::vec3(0.0f, 0.15f, 0.0f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.15f, 0.55f, 0.15f);		// specular reflection
	obj->shininess = .25f;
}

Eyeball::~Eyeball()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}

LineSegment::LineSegment(glm::vec3 p_p1, glm::vec3 p_p2)
{
	changePoints(p_p1, p_p2);
}

LineSegment::~LineSegment()
{
}

void LineSegment::changePoints(glm::vec3 p_p1, glm::vec3 p_p2)
{
	p1 = p_p1;
	p2 = p_p2;
	glm::vec3 vec = p_p2 - p_p1;

	float delta_y = -1.0f * vec.y;
	float delta_x = sqrt(vec.x*vec.x + vec.z * vec.z);
	angle = atan(delta_y / delta_x);
}

void LineSegment::draw(GLuint shaderProgram, glm::mat4 C, glm::vec3 line_color)
{
	model = C;

	// Note that since lines don't have an object coordinate
	// we have to transform it to the camera coordinates directly
	glm::vec4 w_p1 = model * glm::vec4(p1, 1.0f);
	glm::vec4 w_p2 = model * glm::vec4(p2, 1.0f);

	// Get the location of the matrices
	GLuint uProjection = glGetUniformLocation(shaderProgram, "projection");
	GLuint uModel = glGetUniformLocation(shaderProgram, "model");
	GLuint uView = glGetUniformLocation(shaderProgram, "view");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	GLuint loc = glGetUniformLocation(shaderProgram, "draw_line");
	glUniform1i(loc, 1);

	GLuint color = glGetUniformLocation(shaderProgram, "line_color");
	glUniform3f(color, line_color.x, line_color.y, line_color.z );

	glLineWidth(2.5);
	glBegin(GL_LINES);
	glVertex3f(w_p1.x, w_p1.y, w_p1.z);
	glVertex3f(w_p2.x, w_p2.y, w_p2.z);
	glEnd();

	glUniform1i(loc, 0);
}

float LineSegment::getLength()
{
	return glm::length(p2-p1);
}

Bunny::Bunny()
{
	initObj("../bunny.obj");
	obj->k_a = glm::vec3(0.2f, 0.2f, 0.2f);	// ambient reflection 
	obj->k_d = glm::vec3(0.0f, 0.15f, 0.0f);		// diffusion reflection 
	obj->k_s = glm::vec3(0.75f, 0.55f, 0.65f);		// specular reflection
	obj->shininess = .85f;
}

Bunny::~Bunny()
{
	if (obj != nullptr)
	{
		delete obj;
		obj = nullptr;
	}
}
