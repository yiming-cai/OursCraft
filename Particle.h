#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Util.h"

class Particle {
private:
	glm::vec3 pos, speed;
	glm::vec4 color;
	float size, angle, weight;
	float life_time;

public:
	Particle();
	~Particle();

	glm::mat4 toWorld;
	std::vector<Particle> particles;
	int particlesCount;
	void draw(GLuint shaderProgram);
	void update(GLfloat time, GLuint newParticles, glm::vec3 offset);
	GLuint firstUnusedParticle();
	GLuint Particle::loadTexture();
	unsigned char* Particle::loadPPM(const char* filename, int& width, int& height);
	void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	GLuint VAO, VBO, TBO;
	GLuint uProjection, uModelview, uModel;
	GLuint textureID;
};

#endif