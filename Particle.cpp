#define _CRT_SECURE_NO_WARNINGS
#include "Particle.h"
#include <iostream>
const int maxParticles = 1000;
extern glm::mat4 P;
extern glm::mat4 V;
Particle::Particle() {
	toWorld = glm::mat4(1.0f);

	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};
	GLfloat particle_texCoord[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &TBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_texCoord), particle_texCoord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	for (GLint i = 0; i < maxParticles; ++i)
		this->particles.push_back(Particle());
	textureID = loadTexture();

}


Particle::~Particle() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &TBO);
}

void Particle::update(GLfloat time, GLuint newParticles, glm::vec3 offset)
{
	//new
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], offset);
	}
	//update
	for (GLuint i = 0; i < maxParticles; ++i)
	{
		Particle &p = this->particles[i];
		p.life_time -= time; 
		if (p.life_time > 0.0f)
		{	
			p.pos += glm::vec3(p.speed.x*time, p.speed.y*time - 0.5f * 20 * time*time, p.speed.z*time);
			p.color.a -= time * 2.5;
		}
	}
}

void Particle::draw(GLuint shaderProgram) {
	glm::mat4 modelview = V * toWorld;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUseProgram(shaderProgram);
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	for (Particle particle : this->particles)
	{
		if (particle.life_time > 0.0f)
		{
			glUniform3f(glGetUniformLocation(shaderProgram, "offset"), particle.pos.x, particle.pos.y, particle.pos.z);
			glUniform4f(glGetUniformLocation(shaderProgram, "color"), particle.color.x, particle.color.y, particle.color.z, particle.color.w);
			glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint Particle::loadTexture() {
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	/////////image = loadPPM("", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Sets texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Unbinds texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint Particle::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < maxParticles; ++i) {
		if (this->particles[i].life_time <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].life_time <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void Particle::respawnParticle(Particle &particle, glm::vec3 offset)
{
	GLfloat randomx = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomy = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomz = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomSpeedx = ((rand() % 100) - 50) / 100.0f;
	GLfloat randomSpeedy = ((rand() % 100) - 50) / 100.0f;
	GLfloat randomSpeedz = ((rand() % 100) - 50) / 100.0f;

	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.pos = glm::vec3(randomx / 3.0f + offset.x, offset.y, offset.z + randomz / 3.0f);
	particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.life_time = 30.0f;
	particle.speed = glm::vec3(0.2f + randomSpeedx, 1.0f + randomSpeedy, 0.2f + randomSpeedz);
}


unsigned char* Particle::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}