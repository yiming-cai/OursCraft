#include "Cube.h"

std::vector<std::string> Cube::cubeTexturesAddress = {
	"../assets/CubeTexture/cube_01.jpg",
	"../assets/CubeTexture/cube_02.jpg",
	"../assets/CubeTexture/cube_03.jpg",
	"../assets/CubeTexture/cube_04.jpg",
	"../assets/CubeTexture/cube_05.jpg",
	"../assets/CubeTexture/cube_06.jpg",
	"../assets/CubeTexture/cube_07.jpg",
	"../assets/CubeTexture/cube_08.jpg"
};

std::vector<GLint> Cube::cubeTextureId = {
	-1,-1,-1,-1,-1,-1,-1,-1
};

GLfloat Cube::cubeBoundBoxV[72] = {
	0,0,0,  1,0,0,	1,1,0,	0,1,0,		// front face counterclockwise
	0,0,-1, 0,1,-1, 1,1,-1, 1,0,-1,		// back face clockwise

	0,0,-1, 0,0,0, 0,1,0, 0,1,-1,		// left face clockwise
	1,0,-1, 1,1,-1, 1,1,0, 1,0,0,		// right face counterclockwise

	0,1,0, 1,1,0, 1,1,-1, 0,1,-1,		// top face counterclockwise
	0,0,0, 0,0,-1, 1,0,-1,  1,0,0		// bottom face clockwise
};

GLfloat Cube::cubeBoundBoxN[18] = {
	0,0,1,
	0,0,-1,
	-1,0,0,
	1,0,0,
	0,1,0,
	0,-1,0
};

// The order is such that:
// the front face has the first 4 vertices, counterclockwise facing towards camera
// the front face has the last 4 vertices, counterclockwise facing away from camera
int Cube::cubeVerticesLen = 108;
GLfloat Cube::cubeVertices[108] = {
	0,0,0,  1,0,0,	1,1,0,	
	1,1,0, 0,1,0, 0,0,0,	// front face counterclockwise
	0,0,-1, 0,1,-1, 1,1,-1, 
	1,1,-1, 1,0,-1, 0,0,-1,		// back face clockwise

	0,0,-1, 0,0,0, 0,1,0, 
	0,1,0, 0,1,-1, 0,0,-1,		// left face clockwise
	1,0,-1, 1,1,-1, 1,1,0, 
	1,1,0, 1,0,0, 1,0,-1,		// right face counterclockwise

	0,1,0, 1,1,0, 1,1,-1, 
	1,1,-1, 0,1,-1, 0,1,0,		// top face counterclockwise
	0,0,0, 0,0,-1, 1,0,-1, 
	1,0,-1, 1,0,0, 0,0,0		// bottom face clockwise
};

int Cube::cubeColorsLen = 108;

int Cube::cubeNormalsLen = 108;
GLfloat Cube::cubeNormals[108] = {
	0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1,			// FRONT
	0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1		// BACK
	-1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0,		// LEFT
	-1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0,			// RIGHT
	0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0,			// UP
	0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0		// DOWN
};


GLuint Cube::loadTexture(int pos)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; ++i) {
		unsigned char *data = stbi_load(cubeTexturesAddress[pos].c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			//printf("%d %d %d\n", width, height, nrChannels);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			printf("wrong in load skybox %s", cubeTexturesAddress[pos].c_str());
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;

}
Cube::Cube(int id, float size, glm::vec3 color) {
	this->id = id;
	this->selected = 0;
	this->haveTexture = 0;
	this->size = size;
	this->nameID = OBJECT_CUBE;
	this->vertices = cubeVertices;
	for (int i = 0; i < 36; ++i) {
		for (int j = 0; j < 3; ++j)
			cubeColors[3 * i + j] = color[j];
	}
	this->colors = cubeColors;
	this->normals = cubeNormals;

	this->boundBoxN = cubeBoundBoxN;
	this->boundBoxV = cubeBoundBoxV;
	this->shader = Shader_Geometry;
	this->toWorld = glm::scale(glm::mat4(1.0f),glm::vec3(size,size,size));
	verticesLen = cubeVerticesLen;
	normalsLen = cubeNormalsLen;
	colorsLen = cubeColorsLen;
	//printf("in address %d\n", &P);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLen * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normalsLen * sizeof(GLfloat), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colorsLen * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	//printf("Load Cube!\n");
}

Cube::Cube(int id, float size, int textureSeq)
{
	this->id = id;
	this->selected = 0;
	this->haveTexture = 1;
	this->size = size;
	this->nameID = OBJECT_CUBE;
	this->vertices = cubeVertices;
	for (int i = 0; i < 36; ++i) {
		for (int j = 0; j < 3; ++j)
			cubeColors[3 * i + j] = 1;
	}
	this->colors = cubeColors;
	this->normals = cubeNormals;

	this->boundBoxN = cubeBoundBoxN;
	this->boundBoxV = cubeBoundBoxV;
	this->shader = Shader_Geometry;
	this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(size, size, size));
	verticesLen = cubeVerticesLen;
	normalsLen = cubeNormalsLen;
	colorsLen = cubeColorsLen;
	//printf("in address %d\n", &P);
	if (cubeTextureId.size() <= textureSeq) textureSeq = cubeTextureId.size() - 1;
	if (cubeTextureId[textureSeq] == -1) {
		cubeTextureId[textureSeq] = loadTexture(textureSeq);
	}
	textureID = cubeTextureId[textureSeq];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &CBO);
	glGenBuffers(1, &NBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesLen * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normalsLen * sizeof(GLfloat), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, colorsLen * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
	//printf("Load Cube!\n");
}

Cube::~Cube()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &CBO);
}

