#define _CRT_SECURE_NO_WARNINGS
#include "ParticleGenerator.h"
#include <iostream>
const int maxParticles = 1000;
extern glm::mat4 P;
extern glm::mat4 V;
ParticleGenerator::ParticleGenerator() {
	toWorld = glm::mat4(1.0f);

	GLfloat particle_quad[] = {
		0.0f, 1.5f, 0.0f, 
		1.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.5f, 0.0f,
		1.5f, 1.5f, 0.0f,
		1.5f, 0.0f, 0.0f,
	};
	GLfloat particle_texCoord[] = {
		0.0f, 2.0f,
		2.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 2.0f,
		2.0f, 2.0f,
		2.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_texCoord), particle_texCoord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	for (GLint i = 0; i < maxParticles; ++i)
		this->particles.push_back(Particle());
	textureID = loadTexture();

}


ParticleGenerator::~ParticleGenerator() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
}

void ParticleGenerator::update(GLfloat time, GLuint newParticles, glm::vec3 offset)
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
		p.Life -= time;
		if (p.Life > 0.0f)
		{
			p.Position += p.Velocity * time;
			p.Color.a -= time * 2.5;
		}
	}
}

void ParticleGenerator::draw(GLuint shaderProgram) {
	glm::mat4 modelview = V * toWorld;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_CULL_FACE);

	//use shader
	glUseProgram(shaderProgram);
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);

	glBindVertexArray(VAO);
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			glUniform3f(glGetUniformLocation(shaderProgram, "offset"), particle.Position.x, particle.Position.y, particle.Position.z);
			glUniform4f(glGetUniformLocation(shaderProgram, "color"), particle.Color.x, particle.Color.y, particle.Color.z, particle.Color.w);
			glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		}
	}
	glBindVertexArray(0);

	//reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}




// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < maxParticles; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 offset)
{
	GLfloat randomX = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomY = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomZ = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomVelocityX = ((rand() % 100) - 50) / 100.0f;
	GLfloat randomVelocityY = ((rand() % 100) - 50) / 100.0f;
	GLfloat randomVelocityZ = ((rand() % 100) - 50) / 100.0f;

	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = glm::vec3(randomX / 3.0f + offset.x, randomY + offset.y, offset.z + randomZ / 3.0f);
	particle.Velocity = glm::vec3(0.5f + randomVelocityX, 0.5f + randomVelocityY, 0.5f + randomVelocityZ);
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 20.0f;
}


unsigned char* ParticleGenerator::loadPPM(const char* filename, int& width, int& height)
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


GLuint ParticleGenerator::loadTexture() {
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	image = loadPPM("../assets/orange.ppm", width, height);
	std::cout << "\n-----------successful loeading pparticle---------------\n" ;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}