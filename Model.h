#pragma once

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/postprocess.h>		// Post processing flags
#include <assimp/scene.h>           // Output data structure
#include <assimp/mesh.h>
#include <iostream>
#include <fstream>
#include <map>
#include <limits.h>
#include <math.h>
#include <set>
#include <algorithm>

#include "Util.h"
#include "Camera.h"
#include "BoundBox.h"
#include "stb_image.h"

extern glm::mat4 P;
extern glm::mat4 V;

struct MyMaterial {

	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emissive[4];
	float shininess;
	float opacity;
	int texCount;
	int PADDING;
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

/*
* author: Yiming Cai
* source: various opengl references and stack overflow, and tutorials with links below
* date: 12/5/2017
*/
class Model
{
private:
	// import obj: initialiates the scene
	// Source: http://assimp.org
	bool importObj(const std::string& path);

	//void genBuffers();
	//void delBuffers();

	// Generate the VAO and Buffers
	// source: http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
	void genVAOsAndUniformBuffer(const aiScene *sc);
	int loadGLTextures(const aiScene * sc);

	// For mapping texture IDs
	std::map<std::string, GLuint> textureIdMap;
	std::set<GLuint> IDs;

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
	void setMinMaxObjectCoord(const aiScene * sc);

	// define the min_max_pairs_xyz values, in this format:
	// min_max_pairs_xyz[0] : minimum x value
	// min_max_pairs_xyz[1] : maximum x value
	// min_max_pairs_xyz[2] : minimum y value
	// min_max_pairs_xyz[3] : maximum y value
	// min_max_pairs_xyz[4] : minimum z value
	// min_max_pairs_xyz[5] : maximum z value
	std::vector<float> min_max_pairs_xyz;
	std::pair< glm::vec3, float > boundingSphereObjectCoord;
	void setBoundingSphere();

	// this is the matrix that can be used to scale the model to desired scale
	glm::mat4 scale_matrix = glm::mat4(1.0f);

	// Model matrix of the model, set it to whatever you prefer
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	// Unscaled matrix of the model
	glm::mat4 unscaledModelMatrix = glm::mat4(1.0f);

	// each corresponds to one of the mesh, same first index
	std::vector< std::vector<float> > texCoordsArrays;
	std::vector< std::vector<unsigned int> > faceArrays;


public:

	BoundBox* bounding_box = new BoundBox(1.0f, 1.0f, 1.0f);
	void setBoundingBox();

	// constructor, just takes in a file path
	Model(std::string p_filepath);
	~Model();

	// render methods
	// IMPORTANT: call initShader(s) and initCamera(cam) before calling render(s) or draw(C,s)
	void initShader(GLuint shaderProgram );
	void setCamera(Camera * cam);

	void render(GLuint shaderProgram);
	void draw(glm::mat4 C, GLuint shaderProgram);

	// dummy update method, does nothing in this case
	void update();

	/* ---------------------------------------------- */
	// use these when accessing values of getMinMaxValues() Note that these are in object coordinates
	const static int INDEX_X_MIN = 0;
	const static int INDEX_X_MAX = 1;
	const static int INDEX_Y_MIN = 2;
	const static int INDEX_Y_MAX = 3;
	const static int INDEX_Z_MIN = 4;
	const static int INDEX_Z_MAX = 5;

	// note that these values are in object coordinates
	std::vector<float> getMinMaxValues() { return min_max_pairs_xyz; }
	/* ---------------------------------------------- */


	/* ---------------------------------------------- */
	// make the model center (will be in world center if model = glm::mat4(1.0f) ), with its maximum xyz size set by scale
	void centerAndScale(float scale);

	// Turn off the scaling and return to original size/position
	void turnOffScale() { scale_matrix = glm::mat4(1.0f); modelMatrix = unscaledModelMatrix;  }
	/* ---------------------------------------------- */


	/* ----------------------------------------- */
	// BBV = Bounding Box Vertex
	const static int BBV_BOTTOM_LEFT_NEAR = 0;
	const static int BBV_BOTTOM_RIGHT_NEAR = 1;
	const static int BBV_TOP_RIGHT_NEAR = 2;
	const static int BBV_TOP_LEFT_NEAR = 3;
	const static int BBV_BOTTOM_LEFT_FAR = 4;
	const static int BBV_BOTTOM_RIGHT_FAR = 5;
	const static int BBV_TOP_RIGHT_FAR = 6;
	const static int BBV_TOP_LEFT_FAR = 7;

	// Make sure you use the right model matrix if you are rendering/updating multiple objects with different model matrix
	void setModelMatrix(glm::mat4 C) { modelMatrix = C * scale_matrix; unscaledModelMatrix = C; setBoundingBox(); };
	glm::mat4 getModelMatrix() const { return modelMatrix; }
	glm::mat4 getUModelMatrix() const { return unscaledModelMatrix; }
	// Use this for a correct AABB Bounding Box Vertices
	// The returned vertices will be axis-orientated in world coordinates
	std::vector< glm::vec3 > getAABBBoundingBoxVertices();

	// Use this if you DON'T CARE about the computing power, it will take a lot longer to compute
	// Good for static objects that only get transformed one time (don't call this in every single frame!!)
	std::vector< glm::vec3 > getAABBTightestBoundingVertices();

	// Use this if you just want the min and max xxyyzz values
	std::vector< float > getAABBBoundingBoxMinMax();

	// Use this if you don't care about the computing power, it will take a lot longer to compute
	// Good for static objects that only get transformed one time
	std::vector< float > getAABBTightestBoundingBoxMinMax();

	// will return a pair: the sphere center in world coordinates, and the radius of the sphere
	std::pair< glm::vec3, float > getBoundingSphere();
	/* ---------------------------------------- */


	/* -------VVVV DEPRECATED, DO NOT USE (THESE ARE ACTUALLY TIGHT BOUNDING BOXES, NOT AABB) VVVVV--------- */
	/* ---------------------------------------------- */
	// use these when accessing the values of getOBBBoundingPlanes()
	// for example: std::vector<glm::vec3> planes = model->getOBBBoundingPlanes();
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
	std::vector< glm::vec3 > getOBBBoundingPlanes();
	/* ---------------------------------------------- */

	/* ---------------------------------------------- */
	// Note that BBV stands for Bounding Box Vertex
	//	it is the indices of vertices returned by
	//	getOBBBoundingBoxVertices()

	// get all the vertices of the bounding box. The box will be OBB, or Orientated Bounding Box
	std::vector< glm::vec3 > getOBBBoundingBoxVertices();
	/* ---------------------------------------------- */
	/* --------------------------------------------------------------------------------------------------- */
};