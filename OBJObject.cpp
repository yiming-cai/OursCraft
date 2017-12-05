#include "OBJObject.h"


OBJObject::OBJObject(const char *filepath,float pointSize) 
{
	this->angle = this->orbit = 0.0f;
	this->pointSize = pointSize;
	this->xTran = this->yTran = this->zTran = 0;
	this->scaling = 1.0f;
	orbit = orbitX = orbitY = orbitZ = 0;
	toWorld = glm::mat4(1.0f);
	orbitW = glm::mat4(1.0f);
	transW = glm::mat4(1.0f);
	doit = 0;
	parse(filepath);
	unsigned int len = vertices.size();
	mVertices = new GLfloat[len];
	for (unsigned int i = 0; i < len; ++i) {
		mVertices[i] = vertices[i];
	}
	len = normals.size();
	mNormals = new GLfloat[len];
	for (unsigned int i = 0; i < len; ++i) {
		mNormals[i] = normals[i];
	}
	len = colors.size();
	mColors = new GLfloat[len];
	for (unsigned int i = 0; i < len; ++i) {
		mColors[i] = colors[i];
	}
	len = indices.size();
	mIndices = new GLuint[len];
	for (unsigned int i = 0; i < len; ++i) {
		mIndices[i] = indices[i];
	}
 	printf("%d %d\n", sizeof(mVertices), sizeof(mIndices));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &CBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), mVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat), (GLvoid*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), mColors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), mNormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * indices.size(), mIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

OBJObject::~OBJObject() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &CBO);
	glDeleteBuffers(1, &NBO);
}
void OBJObject::parse(const char *filepath) 
{
	FILE *fp;
	int indexx, indexy, indexz, normalx, normaly, normalz;
	float maxX, maxY, maxZ, minX, minY, minZ;
	float x, y, z; // for the position 
	float len;
	float r, g, b; // for the color
	int c1, c2;
	char s[1010];
	maxX = maxY = maxZ = -999999;
	minX = minY = minZ = 999999;
	fopen_s(&fp,filepath, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open the FILE!\n");
		glfwTerminate();
		return;
	}
	
	while (!feof(fp)) {
		c1 = fgetc(fp);
		c2 = fgetc(fp);
		fgets(s, 1000, fp);
		if (c1 == 'v' && c2 == ' ') {
			sscanf_s(s, "%f %f %f %f %f %f", &x, &y, &z,&r,&g,&b);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			minX = MIN(minX, x);
			minY = MIN(minY, y);
			minZ = MIN(minZ, z);
			maxX = MAX(maxX, x);
			maxY = MAX(maxY, y);
			maxZ = MAX(maxZ, z);
			//localSize.push_back(3 / (1 + pow(2.71828,(-z))) + 1);
		}
		else if (c1 == 'v' && c2 == 'n') {
			sscanf_s(s, "%f %f %f", &x, &y, &z);
			normals.push_back(x);
			normals.push_back(y);
			normals.push_back(z);
			len = sqrt(x * x + y * y + z * z);
			x /= len;
			y /= len;
			z /= len;
			x = x / 2 + 0.5f;
			y = y / 2 + 0.5f;
			z = z / 2 + 0.5f;
			colors.push_back(x);
			colors.push_back(y);
			colors.push_back(z);
		}
		else if (c1 == 'f' && c2 == ' ') {
			sscanf_s(s, "%d//%d %d//%d %d//%d", &indexx, &normalx, &indexy, &normaly, &indexz, &normalz);
			indices.push_back(indexx - 1);
			indices.push_back(indexy - 1);
			indices.push_back(indexz - 1);
		//	printf("faces %d//%d %d//%d %d//%d\n", indexx, normalx, indexy, normaly, indexz, normalz);
		}
		
	}
	printf("parse object %s ok!\n",filepath);
	fclose(fp);
	scalingX = 2 / (maxX - minX);
	biasX = (maxX + minX) / (minX - maxX);
	scalingY = 2 / (maxY - minY);
	biasY = (maxY + minY) / (minY - maxY);
	scalingZ = 2 / (maxZ - minZ);
	biasZ = (maxZ + minZ) / (minZ - maxZ);
	midX = scalingX * (maxX + minX) / 2 + biasX;
	midY = scalingY * (maxY + minY) / 2 + biasY;
	midZ = scalingZ * (maxZ + minZ) / 2 + biasZ;
	//TODO parse the OBJ file
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
}

void OBJObject::draw() 
{
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shader, "projection");
	uModelview = glGetUniformLocation(shader, "modelview");
	justModel = glGetUniformLocation(shader, "model");
	glm::vec4 k = glm::vec4(1.0, 1.0, 0, 1.0);
	k = this->toWorld * k;
	//printf("%f %f %f\n", k.x, k.y, k.z);
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(justModel, 1, GL_FALSE, &toWorld[0][0]);
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void OBJObject::accumulateUpdate() {
	if (doit == 0) {
		this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(scalingX, scalingY, scalingZ)) * this->toWorld;
		this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(biasX, biasY, biasZ))  * this->toWorld;
		this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-midX, -midY, -midZ)) * this->toWorld;
		doit = 1;
	}
	// for scrolling 
	if (this->orbit != 0) {
		this->toWorld = glm::rotate(glm::mat4(1.0f), this->orbit / 180.0f * glm::pi<float>(), glm::vec3(orbitX, orbitY, orbitZ)) * toWorld;
		this->orbit = 0;
	}
	if (this->xTran != 0 || this->yTran != 0 || this->zTran != 0)
	{
		this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(xTran, yTran, zTran)) * this->toWorld;
		this->xTran = 0;
		this->yTran = 0;
		this->zTran = 0;
	}

	// for scaling 
	this->toWorld = glm::scale(this->toWorld, glm::vec3(scaling , scaling, scaling));
	scaling = 1;

}

void OBJObject::update()
{
	this->toWorld = glm::mat4(1.0f);
	/*
	this->angle += 1.0f;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	if (this->orbit > 360.0f || this->orbit < -360.0f) this->orbit = 0.0f;
	this->toWorld = glm::rotate(glm::mat4(1.0f), this->orbit / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	this->toWorld = glm::translate(this->toWorld, glm::vec3(xTran, yTran, zTran));
	this->toWorld = glm::rotate(this->toWorld, this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	
	*/
	//printf("%f %f %f\n", midX, midY, midZ);
	// for centering and scalling the model
	
	this->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(scalingX, scalingY, scalingZ)) * this->toWorld;
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(biasX, biasY, biasZ))  * this->toWorld;
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-midX, -midY, -midZ)) * this->toWorld;
	// for scrolling 
	//this->toWorld = this->orbitW * this->toWorld;
	//this->translate();
	this->rotate();
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(xTran, yTran, zTran)) * this->toWorld; 
	
	
	// for scaling 
	this->toWorld = glm::scale(this->toWorld, glm::vec3(scaling, scaling, scaling));
	
//	this->spin(1.0f);

	
}

void OBJObject::translate() {
	if (this->xTran != 0 || this ->yTran !=0 || this ->zTran != 0)
	{

		//if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
		this->transW = glm::translate(glm::mat4(1.0f), glm::vec3(xTran, yTran, zTran)) * this->transW;
		this->xTran = 0;
		this->yTran = 0;
		this->zTran = 0;
	}
	this->toWorld = this->transW  * this->toWorld;
}

void OBJObject::rotate()
{
	if (this->orbit != 0)
	{
		
		//if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
		this-> orbitW = glm::rotate(glm::mat4(1.0f), this->orbit / 180.0f * glm::pi<float>(), glm::vec3(orbitX, orbitY, orbitZ));
		this->orbit = 0;
	}
	this->toWorld = this->orbitW * this->toWorld;

}

void OBJObject::spin(float deg)
{
	this->angle += deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	this->toWorld = this ->toWorld * glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}
