#include "Skybox.h"

GLfloat Skybox::skyboxVertices[108] = {
	-1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f,1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f,-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f,1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,1.0f, -1.0f,  1.0f
};

GLuint Skybox::loadCubemap()
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces -> size(); ++i) {
		unsigned char *data = stbi_load((*faces)[i].c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			//printf("%d %d %d\n", width, height, nrChannels);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			printf("wrong in load skybox %s", (*faces)[i].c_str());
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;

}

Skybox::Skybox(int id, float distance, std::vector<std::string>* faces)
{
	this->id = id;
	this->distance = distance;
	this->faces = faces;
	this->shader = Shader_Skybox;
	texture = loadCubemap();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printf("Skybox Load!\n");
}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &texture);
}

void Skybox::draw(glm::mat4 C)
{
	glm::mat4 modelview = V * C * glm::mat4(1.0f);
	//glDepthMask(GL_FALSE);
	glUseProgram(shader);

	GLuint uProjection = glGetUniformLocation(shader, "projection");
	GLuint uModelview = glGetUniformLocation(shader, "modelview");
	GLuint skyboxI = glGetUniformLocation(shader, "skybox");
	GLuint distanceI = glGetUniformLocation(shader, "distance");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);


	glBindVertexArray(VAO);
	glUniform1i(skyboxI, 0);
	glUniform1f(distanceI, distance);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthMask(GL_TRUE);
	glBindVertexArray(0);
}

void Skybox::update()
{
}
