#pragma once
#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/postprocess.h>		// Post processing flags
#include <assimp/scene.h>           // Output data structure
#include <assimp/mesh.h>
#include "Util.h"
#include <iostream>
#include <fstream>
#include <map>
#include "Node.h"
#include "Camera.h"
#include <limits.h>
#include <math.h>

/*
 * source: http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
 */

extern glm::mat4 P;
extern glm::mat4 V;

class Model
{
private:
	// import obj: initialiates the scene
	bool importObj(const std::string& path);

	//void genBuffers();
	//void delBuffers();

	// Generate the VAO and Buffers
	void genVAOsAndUniformBuffer(const aiScene *sc);

	// For mapping texture IDs
	std::map<std::string, GLuint> textureIdMap;

	// Location in the shader
	const static int vertexLoc = 0;
	const static int normalLoc = 1;
	const static int texCoordLoc = 2;
	const static int materialUniLoc = 3;

	// store material properties
	std::vector<MyMesh> myMeshes;

	// assimp libs
	const aiScene* scene;
	Assimp::Importer importer;
	std::string filepath;

	// camera
	Camera * camera = nullptr;

	// bounding box
	void setBoundingBox(const aiScene * sc);

	// define the AABB values, in this format:
	// AABB stands for Axis-aligned minimum bounding box
	// AABB[0] : minimum x value
	// AABB[1] : maximum x value
	// AABB[2] : minimum y value
	// AABB[3] : maximum y value
	// AABB[4] : minimum z value
	// AABB[5] : maximum z value
	std::vector<float> AABB;

	// this is the matrix that can be used to scale the model to desired scale
	glm::mat4 scale_matrix = glm::mat4(1.0f);

public:

	// Model matrix of the model, set it to whatever you prefer
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	// constructor, just takes in a file path
	Model(std::string p_filepath);
	~Model();

	// render methods
	// IMPORTANT: call initShader(s) and initCamera(cam) before calling render(s) or draw(C,s)
	void initShader(GLuint shaderProgram);
	void setCamera(Camera * cam);
	
	void render(GLuint shaderProgram);
	void draw(glm::mat4 C, GLuint shaderProgram);
	
	// dummy update method, does nothing in this case
	void update();

	/* ---------------------------------------------- */
	// use these when accessing values of getMinMaxValues()
	const static int MODEL_X_MIN = 0;
	const static int MODEL_X_MAX = 1;
	const static int MODEL_Y_MIN = 2;
	const static int MODEL_Y_MAX = 3;
	const static int MODEL_Z_MIN = 4;
	const static int MODEL_Z_MAX = 5;
	
	// note that these values are in object coordinates
	std::vector<float> getMinMaxValues() { return AABB; }
	/* ---------------------------------------------- */
	
	
	/* ---------------------------------------------- */
	// use these when accessing the values of getBoundingPlanes()
	// for example: std::vector<glm::vec3> planes = model->getBoundingPlanes();
	//		planes[NORMAL_LEFT] is the normal of the left plane in glm::vec3 format
	//		planes[POINT_FRONT] is a point on the front plane in glm::vec3 format
	const static int NORMAL_LEFT = 0;
	const static int POINT_LEFT = 1;
	const static int NORMAL_RIGHT = 2;
	const static int POINT_RIGHT = 3;
	const static int NORMAL_BOTTOM = 4;
	const static int POINT_BOTTOM = 5;
	const static int NORMAL_TOP = 6;
	const static int POINT_TOP = 7;
	const static int NORMAL_BACK = 8;
	const static int POINT_BACK = 9;
	const static int NORMAL_FRONT = 10;
	const static int POINT_FRONT = 11;
	
	// this function will return the bounding planes in world
	//	coordinates. Note that all normal vectors will point outwards
	std::vector< glm::vec3 > getBoundingPlanes();
	/* ---------------------------------------------- */
	
	/* ---------------------------------------------- */
	// make the model center, with its maximum xyz size set by scale
	void centerAndScale(float scale);
	// Turn off the scaling and return to original size/position
	void turnOffScale() { scale_matrix = glm::mat4(1.0f); }
	/* ---------------------------------------------- */
	
	/* ---------------------------------------------- */
	// Note that BBV stands for Bounding Box Vertex
	// it is the indices of vertices returned by
	// getBoundingBoxVertices()
	const static int BBV_BOTTOM_LEFT_NEAR = 0;
	const static int BBV_BOTTOM_RIGHT_NEAR = 1;
	const static int BBV_TOP_RIGHT_NEAR = 2;
	const static int BBV_TOP_LEFT_NEAR = 3;
	const static int BBV_BOTTOM_LEFT_FAR = 4;
	const static int BBV_BOTTOM_RIGHT_FAR = 5;
	const static int BBV_TOP_RIGHT_FAR = 6;
	const static int BBV_TOP_LEFT_FAR = 7;
	
	// get all the vertices of the bounding box
	std::vector< glm::vec3 > getBoundingBoxVertices();
	/* ---------------------------------------------- */
};


