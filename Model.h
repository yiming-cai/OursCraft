#pragma once
#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/postprocess.h>		// Post processing flags
#include <assimp/scene.h>           // Output data structure
#include <assimp/mesh.h>
#include "Util.h"
#include <iostream>
#include <fstream>
#include <map>

/*
 * source: http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
 */

struct MyMaterial {

	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emissive[4];
	float shininess;
	int texCount;
};

// Information to render each assimp node
struct MyMesh {

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint nbo;
	GLuint texIndex;
	GLuint uniformBlockIndex;
	int numFaces;
};

extern glm::mat4 P;
extern glm::mat4 V;
extern GLuint Shader_Model;

class Model
{
public:

	// For mapping texture IDs
	std::map<std::string, GLuint> textureIdMap;

	// Location in the shader
	const static int vertexLoc = 0;
	const static int normalLoc = 1;
	const static int texCoordLoc = 2;
	const static int materialUniLoc = 3;

	// store material properties
	std::vector<MyMesh> myMeshes;

	const aiScene* scene;
	Assimp::Importer importer;
	std::string filepath;

	// constructor, just takes in a file path
	Model(std::string p_filepath);
	~Model();

	// import obj: initialiates the scene
	bool importObj(const std::string& path);

	//void genBuffers();
	//void delBuffers();

	// Generate the VAO and Buffers
	void genVAOsAndUniformBuffer(const aiScene *sc);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f));

	void render(const glm::vec3 & cam_pos);

	//void recursive_render(const aiScene * sc, const aiNode * nd);
	

};


