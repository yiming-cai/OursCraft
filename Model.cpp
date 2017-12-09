#include "Model.h"
void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

Model::Model(std::string p_filepath) {
	filepath = p_filepath;
	
	if (importObj(filepath))
	{
		loadGLTextures(scene);
		genVAOsAndUniformBuffer(scene);
		setBoundingBox(scene);
		setBoundingSphere();
	}
	else
	{
		std::cerr << "Failed to load " << filepath << std::endl;
	}
	std::cerr << "Loaded " << myMeshes.size() << " meshes!" << std::endl;
}



Model::~Model()
{
	//delBuffers();
	for (unsigned int i = 0; i < myMeshes.size(); ++i) {

		glDeleteVertexArrays(1, &(myMeshes[i].vao));
		glDeleteTextures(1, &(myMeshes[i].texIndex));
		glDeleteBuffers(1, &(myMeshes[i].uniformBlockIndex));
	}
}

void Model::setDominoBox() {
	//test box
	bounding_box->toWorld = this->getUModelMatrix()* glm::scale(glm::mat4(1.0f), glm::vec3(0.27f, 0.5f, 0.09f));
	bounding_box->update();
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			{

				std::cout << (bounding_box->toWorld)[i][j];
			}
		}
}

/*
 * source: http://www.assimp.org
 */

bool Model::importObj(const std::string& path)
{
	//check if file exists
	std::ifstream fin(path.c_str());
	if (!fin.fail()) {
		fin.close();
	}
	else {
		std::cerr << ("Couldn't open file: %s\n", path.c_str()) << std::endl;
		std::cerr << ("%s\n", importer.GetErrorString()) << std::endl;
		return false;
	}

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs);

	// If the import failed, report it
	if (!scene)
	{
		std::cerr << (importer.GetErrorString()) << std::endl;
		return false;
	}
	return true;
}

//void Model::genBuffers()
//{
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//	glGenBuffers(1, &CBO);
//	glGenBuffers(1, &NBO);
//}

//void Model::delBuffers()
//{
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//	glDeleteBuffers(1, &TBO);
//	glDeleteBuffers(1, &NBO);
//}

/*
* source: http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
*/
void Model::genVAOsAndUniformBuffer(const aiScene *sc) {

	struct MyMesh aMesh;
	struct MyMaterial aMat;
	GLuint buffer;

	// For each mesh
	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const aiMesh* mesh = sc->mMeshes[n];

		// create array with faces
		// have to convert from Assimp format to array
		//unsigned int *faceArray;
		//faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int face_size = mesh->mNumFaces * 3;
		faceArrays.push_back( std::vector<unsigned int> (face_size) );
		int latest = faceArrays.size() - 1;
		
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];

			memcpy(&faceArrays[latest][faceIndex], face->mIndices, 3 * sizeof(unsigned int));
			faceIndex += 3;
		}
		aMesh.numFaces = sc->mMeshes[n]->mNumFaces;

		// generate Vertex Array for mesh
		glGenVertexArrays(1, &(aMesh.vao));
		glBindVertexArray(aMesh.vao);

		// buffer for faces
		glGenBuffers(1, &aMesh.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aMesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * face_size, &(faceArrays[latest][0]), GL_STATIC_DRAW);

		// buffer for vertex positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &aMesh.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, aMesh.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(vertexLoc);
			glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &aMesh.nbo);
			glBindBuffer(GL_ARRAY_BUFFER, aMesh.nbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			glEnableVertexAttribArray(normalLoc);
			glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex texture coordinates
		if (mesh->HasTextureCoords(0)) {
			//float * texCoords = (float *)malloc(sizeof(float) * 2 * mesh->mNumVertices);
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mTextureCoords[0], GL_STATIC_DRAW);
			glVertexAttribPointer(texCoordLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(texCoordLoc);

			//std::cerr << "TEXTURE COORD FOUND!" << std::endl;
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// create material uniform buffer
		aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];

		/* ---------------------- Find texture id of the mesh ----------------------------------- */
		aiString texPath;	//contains filename of texture
		if ( mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0 ) 
		{
			mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

			//bind texture
			unsigned int texId = textureIdMap[texPath.data];
			aMesh.texIndex = texId;
			aMat.texCount = 1;
		}
		else
			aMat.texCount = 0;
		/* --------------------------------------------------------------------------------------- */

		/* -------------------------For parsing material properties -------------------------------- */
		float c[4];
		set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
			color4_to_float4(&diffuse, c);
		memcpy(aMat.diffuse, c, sizeof(c));

		set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
			color4_to_float4(&ambient, c);
		memcpy(aMat.ambient, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D specular;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
			color4_to_float4(&specular, c);
		memcpy(aMat.specular, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D emission;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
			color4_to_float4(&emission, c);
		memcpy(aMat.emissive, c, sizeof(c));

		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
		aMat.shininess = shininess;

		float opacity = 1.0f;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_OPACITY, &opacity, &max);
		aMat.opacity = opacity;
		/* -------------------------------------------------------------------------------------------- */

		/* ------------------------ Send the Material to shader ---------------------------------------- */
		glGenBuffers(1, &(aMesh.uniformBlockIndex));
		glBindBuffer(GL_UNIFORM_BUFFER, aMesh.uniformBlockIndex);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(aMat), (void *)(&aMat), GL_STATIC_DRAW);
		/* --------------------------------------------------------------------------------------------- */
		
		// unbind buffer
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		myMeshes.push_back(aMesh);
	}
}

int Model::loadGLTextures(const aiScene * sc)
{
		bool success;

		/* initialization of DevIL */
		//ilInit();

		///* scan scene's materials for textures */
		//for (unsigned int m = 0; m<scene->mNumMaterials; ++m)
		//{
		//	aiString path;  // filename
		//	for (int i = 0; i < scene->mMaterials[m]->GetTextureCount(aiTextureType_DIFFUSE); i++)
		//	{
		//		scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, i, &path);
		//		
		//		//fill map with textures, OpenGL image ids set to 0
		//		textureIdMap[path.data] = 0;
		//	}
		//}

		// For each mesh
		for (unsigned int n = 0; n < sc->mNumMaterials; ++n)
		{
			const aiMaterial * mat = sc->mMaterials[n];

			/* ---------------------- Find texture id of the mesh ----------------------------------- */
			//std::cerr << "NONE texture: " << (int)mat->GetTextureCount(aiTextureType_NONE) << std::endl;
			//std::cerr << "Diffused texture: " << (int)mat->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
			//std::cerr << "Specular texture: " << (int)mat->GetTextureCount(aiTextureType_SPECULAR) << std::endl;
			//std::cerr << "AMBIENT texture: " << (int)mat->GetTextureCount(aiTextureType_AMBIENT) << std::endl;
			//std::cerr << "EMISSIVE texture: " << (int)mat->GetTextureCount(aiTextureType_EMISSIVE) << std::endl;
			//std::cerr << "HEIGHT texture: " << (int)mat->GetTextureCount(aiTextureType_HEIGHT) << std::endl;
			//std::cerr << "NORMALS texture: " << (int)mat->GetTextureCount(aiTextureType_NORMALS) << std::endl;
			//std::cerr << "SHININESS texture: " << (int)mat->GetTextureCount(aiTextureType_SHININESS) << std::endl;
			//std::cerr << "OPACITY texture: " << (int)mat->GetTextureCount(aiTextureType_OPACITY) << std::endl;
			//std::cerr << "DISPLACEMENT texture: " << (int)mat->GetTextureCount(aiTextureType_DISPLACEMENT) << std::endl;
			//std::cerr << "LIGHTMAP texture: " << (int)mat->GetTextureCount(aiTextureType_LIGHTMAP) << std::endl;
			//std::cerr << "REFLECTION texture: " << (int)mat->GetTextureCount(aiTextureType_REFLECTION) << std::endl;
			//std::cerr << "UNKNOWN texture: " << (int)mat->GetTextureCount(aiTextureType_UNKNOWN) << std::endl;
			for (int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString texPath;
				mat->GetTexture(aiTextureType_DIFFUSE, i, &texPath);
				textureIdMap[texPath.data] = 0;
			}
		}

		if (textureIdMap.size() > 0)
		{
			int numTextures = textureIdMap.size();
			std::cerr << numTextures << std::endl;
			std::vector<GLuint> textureIds(numTextures);
			glGenTextures(numTextures, &(textureIds[0]));

			// some stbi outputs
			int width, height, nrChannels;
			int found = filepath.find_last_of("/\\");
			std::string path = filepath.substr(0,found+1);

			auto itr = textureIdMap.begin();
			for (int i = 0; itr != textureIdMap.end(); ++i, ++itr)
			{
				std::string filename = (*itr).first;

				filename = path + filename;
				std::cerr << "Trying to load texture file: " << filename << " of length " << filename.size() << std::endl;

				(*itr).second = textureIds[i];

				unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 4);
				if (data)
				{
					glBindTexture(GL_TEXTURE_2D, textureIds[i]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					std::cerr << "Loaded texture!" << filename << std::endl;
					stbi_image_free(data);
				}
				else
				{
					std::cerr << "wrong in load texture " << filename << std::endl;
					stbi_image_free(data);
				}
			}

			//return success;
			return true;
		}
		
		return false;
}

void Model::initShader(GLuint shaderProgram)
{
	glUniformBlockBinding(shaderProgram, glGetUniformBlockIndex(shaderProgram, "Material"), materialUniLoc);
}

void Model::render(GLuint shaderProgram)
{
	if (scene == nullptr) return;

	for (auto mesh : myMeshes)
	{
		glUseProgram(shaderProgram);

		// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
		// Consequently, we need to forward the projection, view, and model matrices to the shader programs
		// Get the location of the uniform variables "projection" and "modelview"
		GLuint uProjection = glGetUniformLocation(shaderProgram, "projection");
		GLuint uView = glGetUniformLocation(shaderProgram, "view");
		GLuint uModel = glGetUniformLocation(shaderProgram, "model");
		GLuint uCam = glGetUniformLocation(shaderProgram, "cam_pos");

		//printf("%f %f %f\n", k.x, k.y, k.z);
		// Now send these values to the shader program
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform3fv(uCam, 1, &(camera->camera_pos[0]) );

		// Now draw the cube. We simply need to bind the VAO associated with it.
		glBindVertexArray(mesh.vao);
		glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, mesh.uniformBlockIndex, 0, sizeof(struct MyMaterial));
		// bind texture
		glBindTexture(GL_TEXTURE_2D, mesh.texIndex );
		// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
		glDrawElements(GL_TRIANGLES, mesh.numFaces * 3, GL_UNSIGNED_INT, 0);
		// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers

		glBindVertexArray(0);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

void Model::draw(glm::mat4 C, GLuint shaderProgram)
{
	if (scene == nullptr) return;
	modelMatrix = C * scale_matrix;
	unscaledModelMatrix = C;
	render(shaderProgram);
}

void Model::update()
{

}

void Model::setCamera(Camera * cam)
{
	camera = cam;
}

std::vector<glm::vec3> Model::getOBBBoundingPlanes()
{
	std::vector<glm::vec3> planes(12);
	planes[POINT_LEFT] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], 0.0f, 0.0f, 0.0f) );
	planes[POINT_RIGHT] = glm::vec3(modelMatrix *  glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], 0.0f, 0.0f, 0.0f) );
	planes[POINT_BOTTOM] = glm::vec3(modelMatrix * glm::vec4(0.0f, min_max_pairs_xyz[INDEX_Y_MIN], 0.0f, 0.0f) );
	planes[POINT_TOP] = glm::vec3(modelMatrix *  glm::vec4( 0.0f, min_max_pairs_xyz[INDEX_Y_MAX], 0.0f, 0.0f) );
	planes[POINT_BACK] = glm::vec3(modelMatrix *  glm::vec4( 0.0f, 0.0f, min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	planes[POINT_FRONT] = glm::vec3(modelMatrix *  glm::vec4( 0.0f, 0.0f, min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );

	std::vector<glm::vec3> refPoints_a(12);			// using a bit more memory, but doesn't rly matter
	refPoints_a[POINT_LEFT] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], 0.0f, -1.0f, 0.0f) );
	refPoints_a[POINT_RIGHT] = glm::vec3(modelMatrix *  glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], 0.0f, -1.0f, 0.0f) );
	refPoints_a[POINT_BOTTOM] = glm::vec3(modelMatrix *  glm::vec4(0.0f, min_max_pairs_xyz[INDEX_Y_MIN], -1.0f, 0.0f) );
	refPoints_a[POINT_TOP] = glm::vec3(modelMatrix *  glm::vec4(0.0f, min_max_pairs_xyz[INDEX_Y_MAX], -1.0f, 0.0f) );
	refPoints_a[POINT_BACK] = glm::vec3(modelMatrix *  glm::vec4(1.0f, 0.0f, min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	refPoints_a[POINT_FRONT] = glm::vec3(modelMatrix *  glm::vec4(1.0f, 0.0f, min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );

	std::vector<glm::vec3> refPoints_b(12);			// using a bit more memory, but doesn't rly matter
	refPoints_b[POINT_LEFT] = glm::vec3(modelMatrix *  glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], -1.0f, 0.0f, 0.0f) );
	refPoints_b[POINT_RIGHT] = glm::vec3(modelMatrix *  glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], 1.0f, 0.0f, 0.0f) );
	refPoints_b[POINT_BOTTOM] = glm::vec3(modelMatrix *  glm::vec4(1.0f, min_max_pairs_xyz[INDEX_Y_MIN], 0.0f, 0.0f) );
	refPoints_b[POINT_TOP] = glm::vec3(modelMatrix *  glm::vec4(-1.0f, min_max_pairs_xyz[INDEX_Y_MAX], 0.0f, 0.0f) );
	refPoints_b[POINT_BACK] = glm::vec3(modelMatrix *  glm::vec4(0.0f, -1.0f, min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	refPoints_b[POINT_FRONT] = glm::vec3(modelMatrix *  glm::vec4(0.0f, -1.0f, min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );

	// calculate the normals based on the cross products 
	planes[NORMAL_LEFT] = glm::normalize( glm::cross( (refPoints_a[POINT_LEFT] - planes[POINT_LEFT]), (refPoints_b[POINT_LEFT] - planes[POINT_LEFT])) );
	planes[NORMAL_RIGHT] = glm::normalize(glm::cross((refPoints_a[POINT_RIGHT] - planes[POINT_RIGHT]), (refPoints_b[POINT_RIGHT] - planes[POINT_RIGHT])));
	planes[NORMAL_BOTTOM] = glm::normalize(glm::cross((refPoints_a[POINT_BOTTOM] - planes[POINT_BOTTOM]), (refPoints_b[POINT_BOTTOM] - planes[POINT_BOTTOM])));
	planes[NORMAL_TOP] = glm::normalize(glm::cross((refPoints_a[POINT_TOP] - planes[POINT_TOP]), (refPoints_b[POINT_TOP] - planes[POINT_TOP])));
	planes[NORMAL_BACK] = glm::normalize(glm::cross((refPoints_a[POINT_BACK] - planes[POINT_BACK]), (refPoints_b[POINT_BACK] - planes[POINT_BACK])));
	planes[NORMAL_FRONT] = glm::normalize(glm::cross((refPoints_a[POINT_FRONT] - planes[POINT_FRONT]), (refPoints_b[POINT_FRONT] - planes[POINT_FRONT])));

	return planes;
}

void Model::centerAndScale(float scale)
{
	if (scene == nullptr) return;

	float x_offset = (min_max_pairs_xyz[INDEX_X_MIN] + min_max_pairs_xyz[INDEX_X_MAX]) / -2.0f;
	float y_offset = (min_max_pairs_xyz[INDEX_Y_MIN] + min_max_pairs_xyz[INDEX_Y_MAX]) / -2.0f;
	float z_offset = (min_max_pairs_xyz[INDEX_Z_MIN] + min_max_pairs_xyz[INDEX_Z_MAX]) / -2.0f;
	
	glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(x_offset, y_offset, z_offset));
	std::vector<float> temp;
	temp.push_back(min_max_pairs_xyz[INDEX_X_MIN] + x_offset);
	temp.push_back(min_max_pairs_xyz[INDEX_X_MAX] + x_offset);
	temp.push_back(min_max_pairs_xyz[INDEX_Y_MIN] + y_offset);
	temp.push_back(min_max_pairs_xyz[INDEX_Y_MAX] + y_offset);
	temp.push_back(min_max_pairs_xyz[INDEX_Z_MIN] + z_offset);
	temp.push_back(min_max_pairs_xyz[INDEX_Z_MAX] + z_offset);

	float max = 0.0f;
	for (const float p : temp)
	{
		if (abs(p) > max)
		{
			max = abs(p);
		}
	}

	scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale / max / 2.0f )) * mat;
}

std::vector<glm::vec3> Model::getAABBBoundingBoxVertices()
{
	std::vector< float > min_max_world_pairs_xyz = getAABBBoundingBoxMinMax();
	std::vector< glm::vec3 > points(8);

	// front face
	points[BBV_BOTTOM_LEFT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));
	points[BBV_BOTTOM_RIGHT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));
	points[BBV_TOP_RIGHT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));
	points[BBV_TOP_LEFT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));

	// back face 
	points[BBV_BOTTOM_LEFT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));
	points[BBV_BOTTOM_RIGHT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));
	points[BBV_TOP_RIGHT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));
	points[BBV_TOP_LEFT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));

	return std::vector<glm::vec3>();
}

std::vector<glm::vec3> Model::getAABBTightestBoundingVertices()
{
	std::vector< float > min_max_world_pairs_xyz = getAABBTightestBoundingBoxMinMax();
	std::vector< glm::vec3 > points(8);

	// front face
	points[BBV_BOTTOM_LEFT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));
	points[BBV_BOTTOM_RIGHT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));
	points[BBV_TOP_RIGHT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));
	points[BBV_TOP_LEFT_NEAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MAX], 0.0f));

	// back face 
	points[BBV_BOTTOM_LEFT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));
	points[BBV_BOTTOM_RIGHT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MIN], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));
	points[BBV_TOP_RIGHT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MAX], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));
	points[BBV_TOP_LEFT_FAR] = glm::vec3(modelMatrix * glm::vec4(min_max_world_pairs_xyz[INDEX_X_MIN], min_max_world_pairs_xyz[INDEX_Y_MAX], min_max_world_pairs_xyz[INDEX_Z_MIN], 0.0f));

	return std::vector<glm::vec3>();
}

std::vector<float> Model::getAABBBoundingBoxMinMax()
{
	std::vector<glm::vec3> minmax = getOBBBoundingBoxVertices();
	std::vector<float> x_val, y_val, z_val;
	for (auto& v : minmax)
	{
		v = glm::vec3( modelMatrix *  glm::vec4(v, 0.0f) );
		x_val.push_back(v.x);
		y_val.push_back(v.y);
		z_val.push_back(v.z);
	}

	// sorts ascending order by default
	std::sort(x_val.begin(), x_val.end());
	std::sort(y_val.begin(), y_val.end());
	std::sort(z_val.begin(), z_val.end());

	std::vector<float> ret(6);
	ret[INDEX_X_MIN] = x_val[0];
	ret[INDEX_Y_MIN] = y_val[0];
	ret[INDEX_Z_MIN] = z_val[0];

	ret[INDEX_X_MAX] = x_val[x_val.size() - 1];
	ret[INDEX_Y_MAX] = y_val[y_val.size() - 1];
	ret[INDEX_Z_MAX] = z_val[z_val.size() - 1];

	return ret;
}

std::vector<float> Model::getAABBTightestBoundingBoxMinMax()
{
	std::vector<float> min_max_temp = { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };

	for (unsigned int n = 0; n < scene->mNumMeshes; ++n)
	{
		const aiMesh* mesh = scene->mMeshes[n];

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			const aiVector3D obj_p = mesh->mVertices[i];
			glm::vec3 p(obj_p[0], obj_p[1], obj_p[2]);

			// transform into world coordinates
			p = glm::vec3(modelMatrix * glm::vec4(p, 0.0f));

			if (p[0] < min_max_temp[INDEX_X_MIN])
			{
				min_max_temp[INDEX_X_MIN] = p[0];
			}
			if (p[0] > min_max_temp[INDEX_X_MAX])
			{
				min_max_temp[INDEX_X_MAX] = p[0];
			}
			if (p[1] < min_max_temp[INDEX_Y_MIN])
			{
				min_max_temp[INDEX_Y_MIN] = p[1];
			}
			if (p[1] > min_max_temp[INDEX_Y_MAX])
			{
				min_max_temp[INDEX_Y_MAX] = p[1];
			}
			if (p[2] < min_max_temp[INDEX_Z_MIN])
			{
				min_max_temp[INDEX_Z_MIN] = p[2];
			}
			if (p[2] > min_max_temp[INDEX_Z_MAX])
			{
				min_max_temp[INDEX_Z_MAX] = p[2];
			}
		}
	}
	return min_max_temp;
}

std::pair<glm::vec3, float> Model::getBoundingSphere()
{
	std::pair< glm::vec3, float > boundingSphereWorldCoord;
	boundingSphereObjectCoord.first = glm::vec3(modelMatrix * glm::vec4(boundingSphereObjectCoord.first, 1.0f));
	boundingSphereWorldCoord.second = boundingSphereObjectCoord.second;

	// check if we already ever found a bounding sphere
	return boundingSphereWorldCoord;
}

void Model::setBoundingBox(const aiScene * sc)
{
	min_max_pairs_xyz = { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };

	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const aiMesh* mesh = sc->mMeshes[n];

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			//x = p[0];
			//y = p[1];
			//z = p[2];
			const aiVector3D p = mesh->mVertices[i];

			if (p[0] < min_max_pairs_xyz[INDEX_X_MIN])
			{
				min_max_pairs_xyz[INDEX_X_MIN] = p[0];
			}
			if (p[0] > min_max_pairs_xyz[INDEX_X_MAX])
			{
				min_max_pairs_xyz[INDEX_X_MAX] = p[0];
			}
			if (p[1] < min_max_pairs_xyz[INDEX_Y_MIN])
			{
				min_max_pairs_xyz[INDEX_Y_MIN] = p[1];
			}
			if (p[1] > min_max_pairs_xyz[INDEX_Y_MAX])
			{
				min_max_pairs_xyz[INDEX_Y_MAX] = p[1];
			}
			if (p[2] < min_max_pairs_xyz[INDEX_Z_MIN])
			{
				min_max_pairs_xyz[INDEX_Z_MIN] = p[2];
			}
			if (p[2] > min_max_pairs_xyz[INDEX_Z_MAX])
			{
				min_max_pairs_xyz[INDEX_Z_MAX] = p[2];
			}
		}
	}
}

void Model::setBoundingSphere()
{
	float radius = 0.0f;
	glm::vec3 centers;
	glm::vec3 offsets;
	for (int i = 0; i < 3; i++)
	{
		centers[i] = (min_max_pairs_xyz[2 * i] + min_max_pairs_xyz[2 * i + 1])/2.0f;
	}

	std::vector<float> temp;
	for (int i = 0; i < 3; i++)
	{
		temp.push_back(min_max_pairs_xyz[2 * i] - centers[i]);
		temp.push_back(min_max_pairs_xyz[2 * i + 1] - centers[i]);
	}

	float rad = 0.0f;
	for (const float p : temp)
	{
		if (abs(p) > rad)
		{
			rad = abs(p);
		}
	}

	boundingSphereObjectCoord = std::make_pair(centers, rad);
}

std::vector< glm::vec3 > Model::getOBBBoundingBoxVertices()
{
	std::vector< glm::vec3 > points(8);
	
	// front face
	points[BBV_BOTTOM_LEFT_NEAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], min_max_pairs_xyz[INDEX_Y_MIN], min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );
	points[BBV_BOTTOM_RIGHT_NEAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], min_max_pairs_xyz[INDEX_Y_MIN], min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );
	points[BBV_TOP_RIGHT_NEAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], min_max_pairs_xyz[INDEX_Y_MAX], min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );
	points[BBV_TOP_LEFT_NEAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], min_max_pairs_xyz[INDEX_Y_MAX], min_max_pairs_xyz[INDEX_Z_MAX], 0.0f) );
	
	// back face 
	points[BBV_BOTTOM_LEFT_FAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], min_max_pairs_xyz[INDEX_Y_MIN], min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	points[BBV_BOTTOM_RIGHT_FAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], min_max_pairs_xyz[INDEX_Y_MIN], min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	points[BBV_TOP_RIGHT_FAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MAX], min_max_pairs_xyz[INDEX_Y_MAX], min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	points[BBV_TOP_LEFT_FAR] = glm::vec3( modelMatrix * glm::vec4(min_max_pairs_xyz[INDEX_X_MIN], min_max_pairs_xyz[INDEX_Y_MAX], min_max_pairs_xyz[INDEX_Z_MIN], 0.0f) );
	
	return points;
}
