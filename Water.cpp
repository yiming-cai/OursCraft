#include "Water.h"

std::vector<std::string> Water::waterTexturesAddress = {
	"../assets/water.jpg"
};


GLuint Water::loadTexture(int pos)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(waterTexturesAddress[pos].c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		//printf("%d %d %d\n", width, height, nrChannels);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	else {
		printf("wrong in load skybox %s", waterTexturesAddress[pos].c_str());
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;

}

Water::Water(int id, int size, float total, float v, float u, float t, int tex)
{
	this->id = id;
	this->size = size;
	this->total = total;
	this->water_v = v;
	this->water_t = t;
	this->water_u = u;
	this->haveTexture = 1;
	if (tex >= waterTexturesAddress.size() || tex < 0) tex = 0;
	this->textureID = loadTexture(tex);
	this->toWorld = glm::mat4(1.0f);
	this->shader = Shader_Water;
	srand((unsigned)time(NULL));
	float d = float(size) / (total - 1);
	f1 = v * v * t * t / (d * d);
	f2 = 1.0f / (u * t + 2);
	k1 = (4.0f - 8.0f * f1) * f2;
	k2 = (u * t - 2) * f2;
	k3 = 2.0f * f1 * f2;
	verticesLen = colorsLen = normalsLen = (total - 1) * (total - 1) * 18;
	vertices = new GLfloat[(total - 1) * (total - 1) * 18];
	colors = new GLfloat[(total - 1) * (total - 1) * 18];
	normals = new GLfloat[(total - 1) * (total - 1) * 18];
	buffer = new glm::vec3[total * total];
	buffer2 = new GLfloat[total * total];
	buffer3 = new GLfloat[total * total];
	glm::vec3 color = glm::vec3(0, 0, 0);
	float x, y, z;
	int seq, ra;
	for (int i = 0; i < total; ++i) {
		z = i * (-d);
		for (int j = 0; j < total; ++j) {
			x = j * d;
			seq = (i * total + j);
			y = (rand() % 2) * WATER_HEIGHT;
			if (i == 0 || i == total - 1 || j == 0 || j == total - 1) y = 0;
			buffer[seq] = glm::vec3(x, y, z);
			buffer2[seq] = 0;
			buffer3[seq] = 0;
		}
	}
	int ns;
	for (int i = 0; i < total - 1; ++i) {
		for (int j = 0; j < total - 1; ++j) {
			// 0,0
			seq = 18 * (i * (total - 1) + j);
			ns = i * total + j;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,0
			seq += 3;
			ns = i * total + j + 1;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 0,1
			seq += 3;
			ns = (i + 1) * total + j;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,0
			seq += 3;
			ns = i * total + j + 1;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,1
			seq += 3;
			ns = (i + 1) * total + j + 1;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 0,1
			seq += 3;
			ns = (i + 1) * total + j;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLen * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normalsLen * sizeof(GLfloat), normals, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colorsLen * sizeof(GLfloat), colors, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


}


Water::Water(int id, int size, float total, float v, float u, float t,glm::vec3 color)
{
	this->id = id;
	this->size = size;
	this->total = total;
	this->water_v = v;
	this->water_t = t;
	this->water_u = u;
	this->haveTexture = 0;
	this->waterColor = color;
	this->toWorld = glm::mat4(1.0f);
	this->shader = Shader_Water;
	srand((unsigned)time(NULL));
	float d = float(size) / (total - 1);
	f1 = v * v * t * t / (d * d);
	f2 = 1.0f / (u * t + 2);
	k1 = (4.0f - 8.0f * f1) * f2;
	k2 = (u * t - 2) * f2;
	k3 = 2.0f * f1 * f2;
	verticesLen = colorsLen = normalsLen = (total - 1) * (total - 1) * 18;
	vertices = new GLfloat[(total - 1) * (total - 1) * 18];
	colors = new GLfloat[(total - 1) * (total - 1) * 18];
	normals = new GLfloat[(total - 1) * (total - 1) * 18];
	buffer = new glm::vec3[total * total];
	buffer2 = new GLfloat[total * total];
	buffer3 = new GLfloat[total * total];
	float x, y, z;
	int seq,ra;
	for (int i = 0; i < total; ++i) {
		z = i * (-d);
		for (int j = 0; j < total; ++j) {
			x = j * d;
			seq = (i * total + j);
			y = (rand() % 2) * WATER_HEIGHT;
			if (i == 0 || i == total - 1 || j == 0 || j == total - 1) y = 0;
			buffer[seq] = glm::vec3(x, y, z);
			buffer2[seq] = 0;
			buffer3[seq] = 0;
		}
	}
	int ns;
	for (int i = 0; i < total - 1; ++i) {
		for (int j = 0; j < total - 1; ++j) {
			// 0,0
			seq = 18 * (i * (total - 1) + j);
			ns = i * total + j;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,0
			seq += 3;
			ns = i * total + j + 1;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 0,1
			seq += 3;
			ns = (i + 1) * total + j;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,0
			seq += 3;
			ns = i * total + j + 1;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,1
			seq += 3;
			ns = (i + 1) * total + j + 1;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 0,1
			seq += 3;
			ns = (i + 1) * total + j;
			colors[seq] = color.x; colors[seq + 1] = color.y; colors[seq + 2] = color.z;
			vertices[seq] = buffer[ns].x; vertices[seq + 1] = buffer[ns].y; vertices[seq + 2] = buffer[ns].z;
			normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLen * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normalsLen * sizeof(GLfloat), normals, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colorsLen * sizeof(GLfloat), colors, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


}

Water::~Water()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &CBO);
}

void Water::waterUpdate()
{
	int seq, ns;
	for (int i = 0; i < total; ++i) {
		for (int j = 0; j < total; ++j) {
			seq = (i * total + j);
			buffer3[seq] = buffer2[seq];
			buffer2[seq] = buffer[seq].y;
		}
	}
	int count = 0;
	for (int i = 1; i < total - 1; ++i) {
		for (int j = 1; j < total - 1; ++j) {
			seq = (i * total + j);
			buffer[seq].y = k1 * buffer2[seq] + k2 * buffer3[seq] + k3 * (
				((j == total - 1) ? 0 : buffer2[seq + 1]) + 
				((j == 0) ? 0 : buffer2[seq - 1]) + 
				((i == total - 1) ? 0 :buffer2[seq + int(total)]) + 
				((i == 0) ? 0 : buffer2[seq - int(total)]));
			if (buffer[seq].y <= 0.1) ++count;
		}
	}
	//printf("%d\n", count);
	/*
	if (rand() % 10 == 1) {
		for (int i = 0; i < total; ++i) {
			seq = i * total;
			buffer[seq].y = WATER_HEIGHT;
		}
	}
	*/
	
	for (int i = 0; i < total - 1; ++i) {
		for (int j = 0; j < total - 1; ++j) {
			// 0,0
			seq = 18 * (i * (total - 1) + j);
			ns = i * total + j;
			vertices[seq + 1] = buffer[ns].y;
			//normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,0
			seq += 3;
			ns = i * total + j + 1;
			vertices[seq + 1] = buffer[ns].y;
			//normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 0,1
			seq += 3;
			ns = (i + 1) * total + j;
			vertices[seq + 1] = buffer[ns].y;
			//normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,0
			seq += 3;
			ns = i * total + j + 1;
			vertices[seq + 1] = buffer[ns].y;
		//	normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 1,1
			seq += 3;
			ns = (i + 1) * total + j + 1;
			vertices[seq + 1] = buffer[ns].y;
			//normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
			// 0,1
			seq += 3;
			ns = (i + 1) * total + j;
			vertices[seq + 1] = buffer[ns].y;
			//normals[seq] = 0; normals[seq + 1] = 1; normals[seq + 2] = 0;
		}
	}
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLen * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normalsLen * sizeof(GLfloat), normals, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colorsLen * sizeof(GLfloat), colors, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void Water::draw(glm::mat4 C) {

	glUseProgram(shader);
	glm::mat4 model = C * toWorld;
	glm::vec3 cam_pos = glm::vec3(V[0][3], V[1][3], V[2][3]);

	//glm::vec4 temp = glm::vec4(vertices[6], vertices[7], vertices[8], 1.0f);
	uProjection = glGetUniformLocation(shader, "projection");
	uModel = glGetUniformLocation(shader, "model");
	uView = glGetUniformLocation(shader, "view");
	GLuint sel = glGetUniformLocation(shader, "selected");
	GLuint uCam = glGetUniformLocation(shader, "cam_pos");
	if (haveTexture)
	{
		GLuint uSize = glGetUniformLocation(shader, "size");
		glUniform1f(uSize, size);
		GLuint haveT = glGetUniformLocation(shader, "haveTexture");
		glUniform1i(haveT, haveTexture);
	}
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniform3fv(uCam, 1, &cam_pos[0]);
	glUniform1i(sel, selected);

	// optimization: to avoid buffering same vao all the time
	// !!note, don't use this unless you are drawing the same object over and over again
	// Basically, it is expensive to keep on binding and unbinding VAO, so we reduce that
	if (!drawingCube)
	{
		glBindVertexArray(VAO);
	}
	else if (!bindedCubeVAO)
	{
		glBindVertexArray(VAO);
		bindedCubeVAO = true;
	}
	if (haveTexture) glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, verticesLen / 3);
	//glBindVertexArray(0);

	if (haveTexture)
	{
		GLuint haveT = glGetUniformLocation(shader, "haveTexture");
		glUniform1i(haveT, 0);
	}
	selected = 0;
}
