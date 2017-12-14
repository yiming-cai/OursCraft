#include "Util.h"

glm::mat4 P;
glm::mat4 V;
GLuint Shader_Geometry;
GLuint Shader_Skybox;
GLuint Shader_Coordinate;
GLuint Shader_Model;
GLuint Shader_SimplePointer;
GLuint Shader_BoundBox;
GLuint Shader_DisplayLight;
GLuint Shader_Water;
GLuint Shader_Particle;
GLuint Shader_Tree;
// use this for optimization of cube drawing only
// check Geometry draw function to find out more
bool bindedCubeVAO;
bool drawingCube;

std::vector<std::string> Util::split(const std::string & s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (std::getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}
