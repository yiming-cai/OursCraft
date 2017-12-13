#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

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

const struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	float size, angle, weight, Life;
	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


class ParticleGenerator {
public:
	//store the particle 
	std::vector<Particle> particles;
	ParticleGenerator();
	~ParticleGenerator();

	glm::mat4 toWorld;

	int particlesCount;
	void draw(GLuint shaderProgram);
	void update(GLfloat time, GLuint newParticles, glm::vec3 offset);
	GLuint ParticleGenerator::loadTexture();
	unsigned char* ParticleGenerator::loadPPM(const char* filename, int& width, int& height);
	GLuint firstUnusedParticle();
	void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));

	GLuint VAO, VBO, NBO;
	GLuint uProjection, uModelview, uModel;
	GLuint textureID;
};

#endif