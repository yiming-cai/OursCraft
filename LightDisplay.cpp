#include "LightDisplay.h"

LightDisplay::LightDisplay(Light * light, Camera * cam) :
	lights(light), camera(cam)
{
	pointLight = new Model("../assets/lights/pointlight.obj");
	spotLight = new Model("../assets/lights/spotlight.obj");
	pointLight->setCamera(cam);
	spotLight->setCamera(cam);
	spotLight->centerAndScale(1.0f);
	pointLight->centerAndScale(1.0f);

}

LightDisplay::~LightDisplay()
{
	if (pointLight != nullptr)
	{
		delete pointLight;
	}
	if (spotLight != nullptr)
	{
		delete spotLight;
	}
}

void LightDisplay::initShader(GLuint shaderProgram)
{
	if (camera == nullptr || lights == nullptr)
	{
		std::cerr << "Didn't initialize camera and lights" << std::endl;
		return;
	}
	pointLight->initShader(shaderProgram);
	spotLight->initShader(shaderProgram);
	lights->initializeShader(shaderProgram);
	lights->updateShader(shaderProgram);
	light_loc[shaderProgram] = glGetUniformLocation(shaderProgram, "current_light_index");
}

void LightDisplay::update(GLuint shaderProgram)
{
	if (camera == nullptr || lights == nullptr)
	{
		std::cerr << "Didn't initialize camera and lights" << std::endl;
		return;
	}
	positions_and_types = lights->getAllPositionsAndTypes();
	coneDirections = lights->getAllConeDirections();
	lightStatus = lights->getAllStatus();
	lights->updateShader(shaderProgram);
}

void LightDisplay::render(GLuint shaderProgram)
{
	if (camera == nullptr || lights == nullptr)
	{
		std::cerr << "Didn't initialize camera and lights" << std::endl;
		return;
	}
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int i = 0;
	for (auto const& pair : positions_and_types)
	{

		glm::vec3 position = pair.first;
		int type = pair.second;
		
		//if (lightStatus[i] == Light::STATUS_OFF)
		//{
		//	glUniform1i(light_loc[shaderProgram], 1);
		//	i++;
		//	continue;
		//}

		glUniform1i(light_loc[shaderProgram], i);

		if (type == Light::TYPE_POINT)
		{
			pointLight->draw(glm::translate(glm::mat4(1.0f), position), shaderProgram);
		}
		else if (type == Light::TYPE_SPOT)
		{
			spotLight->draw( glm::translate(glm::mat4(1.0f), position), shaderProgram);
		}
		i++;
	}
	glDisable(GL_BLEND); 
}
