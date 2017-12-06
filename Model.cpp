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

Model::Model(std::string p_filepath)
{
	filepath = p_filepath;
	if (importObj(filepath))
	{
		genVAOsAndUniformBuffer(scene);
	}
	std::cerr << "\nLoaded " << myMeshes.size() << " meshes!" << std::endl;
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
		std::cerr << ("Couldn't open file: %s\n", path.c_str());
		std::cerr << ("%s\n", importer.GetErrorString());
		return false;
	}

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		std::cerr << ( importer.GetErrorString() );
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

	glUniformBlockBinding(Shader_Model, glGetUniformBlockIndex(Shader_Model, "Material"), materialUniLoc);

	// For each mesh
	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const aiMesh* mesh = sc->mMeshes[n];

		// create array with faces
		// have to convert from Assimp format to array
		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
			faceIndex += 3;
		}
		aMesh.numFaces = sc->mMeshes[n]->mNumFaces;

		// generate Vertex Array for mesh
		glGenVertexArrays(1, &(aMesh.vao));
		glBindVertexArray(aMesh.vao);

		// buffer for faces
		glGenBuffers(1, &aMesh.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aMesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

		// buffer for vertex positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &aMesh.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, aMesh.vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &aMesh.nbo);
			glBindBuffer(GL_ARRAY_BUFFER, aMesh.nbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex texture coordinates
		if (mesh->HasTextureCoords(0)) {
			float *texCoords = (float *)malloc(sizeof(float) * 2 * mesh->mNumVertices);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

				texCoords[k * 2] = mesh->mTextureCoords[0][k].x;
				texCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;

			}
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(texCoordLoc);
			glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, 0, 0, 0);
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// create material uniform buffer
		aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];

		aiString texPath;	//contains filename of texture
		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
			//bind texture
			unsigned int texId = textureIdMap[texPath.data];
			aMesh.texIndex = texId;
			aMat.texCount = 1;
		}
		else
			aMat.texCount = 0;

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

		glGenBuffers(1, &(aMesh.uniformBlockIndex));
		glBindBuffer(GL_UNIFORM_BUFFER, aMesh.uniformBlockIndex);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(aMat), (void *)(&aMat), GL_STATIC_DRAW);
		
		// unbind buffer
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		myMeshes.push_back(aMesh);
	}

}

void Model::render()
{
	for (auto mesh : myMeshes)
	{
		glUseProgram(Shader_Model);

		// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
		// Consequently, we need to forward the projection, view, and model matrices to the shader programs
		// Get the location of the uniform variables "projection" and "modelview"
		GLuint uProjection = glGetUniformLocation(Shader_Model, "projection");
		GLuint uView = glGetUniformLocation(Shader_Model, "view");
		GLuint uModel = glGetUniformLocation(Shader_Model, "model");
		GLuint uCam = glGetUniformLocation(Shader_Model, "cam_pos");

		//printf("%f %f %f\n", k.x, k.y, k.z);
		// Now send these values to the shader program
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
		glUniform3fv(uCam, 1, &(camera->camera_pos[0]) );

		// Now draw the cube. We simply need to bind the VAO associated with it.
		glBindVertexArray(mesh.vao);
		// bind material uniform
		glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, mesh.uniformBlockIndex, 0, sizeof(struct MyMaterial));
		// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
		glDrawElements(GL_TRIANGLES, mesh.numFaces * 3, GL_UNSIGNED_INT, 0);
		// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
		glBindVertexArray(0);
	}
}

void Model::draw(glm::mat4 C)
{
	model = C;
	render();
}

void Model::update()
{

}

void Model::setCamera(Camera * cam)
{
	camera = cam;
}
