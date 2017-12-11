#include "Fog.h"

Fog::Fog(glm::vec3 color, Camera * camera, float start, float end, float rate)
{
	this->color = color;
	this->camera = camera;
	this->start = start;
	this->end = end;
	this->tempend = end;
	this->rate = rate;
	this->shaderList = new std::vector<GLuint>();
}

void Fog::setCamera(Camera * camera)
{
	this->camera = camera;
}

void Fog::setColor(glm::vec3 color)
{
	this->color = color;
	
}

void Fog::addShader(GLuint shader)
{
	int i;
	for (i = 0; i < this->shaderList->size(); ++i) {
		if ((*shaderList)[i] == shader) break;
	}
	if (i >= this->shaderList->size()) {
		this->shaderList->push_back(shader);
	}
}

void Fog::fogUpdate(int available)
{
	GLuint pos;
	if (tempend <= start + 0.2) rate = fabs(rate);
	if (tempend >= end) rate = -fabs(rate);
	tempend += rate;
	for (int i = 0; i < shaderList->size(); ++i) {
		glUseProgram((*shaderList)[i]);
		pos = glGetUniformLocation((*shaderList)[i], "disableFog");
		glUniform1i(pos, available);
		pos = glGetUniformLocation((*shaderList)[i], "fog_color");
		glUniform3fv(pos, 1, &color[0]);
		pos = glGetUniformLocation((*shaderList)[i], "fog_pos");
		glUniform3fv(pos, 1, &((camera->camera_pos)[0]));
		pos = glGetUniformLocation((*shaderList)[i], "fog_end");
		glUniform1f(pos, tempend);
		pos = glGetUniformLocation((*shaderList)[i], "fog_start");
		glUniform1f(pos, start);
				
	}
}

void Fog::setRate(float rate)
{
	this->rate = rate;
}
