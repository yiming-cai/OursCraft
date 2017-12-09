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
	needUpdate = true;
}

void LightDisplay::render(GLuint shaderProgram)
{
	if (needUpdate)
	{
		lights->updateShader(shaderProgram);
	}

	if (camera == nullptr || lights == nullptr)
	{
		std::cerr << "Didn't initialize camera and lights" << std::endl;
		return;
	}
	//glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int i = 0;
	for (auto const& pair : positions_and_types)
	{
		glm::vec3 position = pair.first;
		int type = pair.second;
		
		if (lightStatus[i] == Light::STATUS_OFF || type == Light::TYPE_DIRECTIONAL)
		{
			glUniform1i(light_loc[shaderProgram], i);
			i++;
			continue;
		}

		glUniform1i(light_loc[shaderProgram], i);

		if (type == Light::TYPE_POINT)
		{
			pointLight->draw(glm::translate(glm::mat4(1.0f), position), shaderProgram);
		}
		else if (type == Light::TYPE_SPOT)
		{
			glm::vec3 defaultAxis = glm::vec3(0, -1.0f, 0);
			glm::vec3 axis = glm::cross(defaultAxis, glm::normalize(coneDirections[i]));
			float angle = acos(glm::dot(defaultAxis, glm::normalize(coneDirections[i])));
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, axis);
			spotLight->draw( glm::translate(glm::mat4(1.0f), position) * rot, shaderProgram);
		}
		i++;

	}
	glUniform1i(light_loc[shaderProgram], 0);
	//glDisable(GL_BLEND); 
}
