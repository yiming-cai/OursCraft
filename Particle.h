#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#define GLFW_INCLUDE_GLEXT
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

const struct Particle {
	glm::vec3 pos, speed;
	glm::vec4 color;
	float size, angle, weight;
	float life_time;	 
	Particle() : pos(0.0f), speed(0.0f), color(1.0f), life_time(0.0f) {}
};

class Particles {
public:
	glm::mat4 toWorld;
	std::vector<Particle> particles;
	int particlesCount;

	Particles();
	~Particles();
	void draw(GLuint shaderProgram);
	void update(GLfloat dt, GLuint newParticles, glm::vec3 offset);
	GLuint firstUnusedParticle();
	GLuint Particles::loadTexture();
	unsigned char* Particles::loadPPM(const char* filename, int& width, int& height);
	void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	GLuint VAO, VBO, TBO;
	GLuint uProjection, uModelview, uModel;
	GLuint textureID;
};

#endif