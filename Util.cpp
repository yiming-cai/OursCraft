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

// use this for optimization of cube drawing only
// check Geometry draw function to find out more
bool bindedCubeVAO;
bool drawingCube;