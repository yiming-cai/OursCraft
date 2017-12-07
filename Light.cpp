#include "Light.h"



glm::vec3 Light::localTrackBallMapping(double xpos, double ypos, int width, int height)
{
	glm::vec3 v;    // Vector v is the synthesized 3D position of the mouse location on the trackball
	float d;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
	v.x = (2.0f* (float)xpos - (float)width) / (float)width;   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
	v.y = ((float)height - 2.0f* (float)ypos) / (float)height;   // this does the equivalent to the above for the mouse Y position
	v.z = 0.0f;   // initially the mouse z position is set to zero, but this will change below
	d = glm::length(v);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
	d = (d<1.0f) ? d : 1.0f;   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
	v.z = sqrtf(1.001f - d*d);  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
	glm::normalize(v);
	return v;  // return the mouse location on the surface of the trackball
}


// ---------------------------------------- public methods underneath ----------------------------------------------

Light::Light()
{
	lights = std::vector<LightParameters>(NUM_LIGHTS);
}

void Light::presetInit()
{
	srand(srand_seed);
	for (int i = 0; i < lights.size(); i++)
	{
		// randomly initialize light types
		int rand_type = rand() % 3;
		switch (rand_type)
		{
		case Light::TYPE_DIRECTIONAL:
			set_preset_directional(i);
			break;

		case Light::TYPE_POINT:
			set_preset_point(i);
			break;

		case Light::TYPE_SPOT:
			set_preset_spot(i);
			break;
		}
	}
}

void Light::update()
{
	
}

void Light::setSeed(int seed)
{
	srand_seed = seed;
}

void Light::randInit()
{
	srand(srand_seed);
	for (int i = 0; i < lights.size(); i++)
	{
		// randomly initialize light types
		int rand_type = rand() % 3;
		switch (rand_type)
		{
		case Light::TYPE_DIRECTIONAL:
			set_preset_directional(i);

			// randomly sets the light direction
			setAbsoluteLightDirection(
				i,
				glm::vec3
				(
				(float)(rand() % 100) / 100.0f,
					(float)(rand() % 100) / 100.0f,
					(float)(rand() % 100) / 100.0f
				)
			);

			// randomly sets the light intensity
			setAbsoluteLightIntensity(
				i,
				glm::vec3
				(
				(float)(rand() % 100) / 100.0f,
					(float)(rand() % 100) / 100.0f,
					(float)(rand() % 100) / 100.0f
				)
			);

			// randomly sets the ambient coefficient
			setAbsoluteAmbientCoefficient(
				i,
				(float) (rand() % 1000) / 1000.0f
			);

			break;

		case Light::TYPE_POINT:
			set_preset_point(i);

			// randomly sets the light position
			setAbsoluteLightPosition(
				i,
				glm::vec3
				(
				(float)(rand() % 300) / 10.0f - 15.0f,
					(float)(rand() % 300) / 10.0f,
					(float)(rand() % 300) / 10.0f - 15.0f
				)
			);

			// randomly sets the light intensity
			setAbsoluteLightIntensity(
				i,
				glm::vec3
				(
				(float)(rand() % 100) / 40.0f,
					(float)(rand() % 100) / 40.0f,
					(float)(rand() % 100) / 40.0f
				)
			);

			// randomly sets the ambient coefficient
			setAbsoluteAmbientCoefficient(
				i,
				(float)(rand() % 1000) / 1000.0f
			);

			break;

		case Light::TYPE_SPOT:
			set_preset_spot(i);

			// randomly sets the light position
			setAbsoluteLightPosition(
				i,
				glm::vec3
				(
				(float)(rand() % 200) / 10.0f - 10.0f,
					(float)(rand() % 200) / 10.0f,
					(float)(rand() % 300) / 10.0f - 10.0f
				)
			);

			// randomly sets the light intensity
			setAbsoluteLightIntensity(
				i,
				glm::vec3
				(
				(float)(rand() % 100) / 20.0f,
					(float)(rand() % 100) / 20.0f,
					(float)(rand() % 100) / 20.0f
				)
			);

			// make it point to the origin, or center of the map
			setAbsoluteConeDirection(i, glm::vec3(0.0f) - getLightPosition(i));

			// make it randomly wide, as wide as up to 120 degrees
			setAbsoluteConeAngleDegrees(i, (float)(rand() % 120));
			break;
		}
	}
}


Light::~Light()
{

}

void Light::set_preset_directional(int index)
{
	lights[index] = DIRECTIONAL_PRESET;
}

void Light::set_preset_point(int index)
{
	lights[index] = POINTLIGHT_PRESET;
}

void Light::set_preset_spot(int index)
{
	lights[index] = SPOTLIGHT_PRESET;
}

void Light::initializeShader(GLuint shaderProgram)
{
	// bind the loc to the shaderprogram 
	glUniformBlockBinding(shaderProgram, glGetUniformBlockIndex(shaderProgram, "LightBlock"), LIGHT_UNIFORM_LOC);

	// generate a new buffer for this shaderprogram
	lightUniformBlocks.emplace(shaderProgram, 0);
	glGenBuffers(1, &(lightUniformBlocks[shaderProgram]) );
	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBlocks[shaderProgram]);
	glBufferData(GL_UNIFORM_BUFFER, NUM_LIGHTS * sizeof(LightParameters), (void *)(&(lights[0])), GL_STATIC_DRAW);

	// update buffer for the first time
	updateShader(shaderProgram);
}

void Light::updateShader(GLuint shaderProgram)
{
	glUseProgram(shaderProgram);
	glBindBufferRange(GL_UNIFORM_BUFFER, LIGHT_UNIFORM_LOC, lightUniformBlocks[shaderProgram], 0, NUM_LIGHTS * sizeof(LightParameters));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, NUM_LIGHTS * sizeof(LightParameters), (void *)(&(lights[0])));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//bool Light::partialUpdateShader(GLuint shaderProgram, int start_index, int num_lights)
//{
//	if (start_index + num_lights > NUM_LIGHTS)
//	{
//		std::cerr << "Trying to update too many lights!" << std::endl;
//		return false;
//	}
//
//	glUseProgram(shaderProgram);
//	glBindBufferRange(
//		GL_UNIFORM_BUFFER,								// type of buffer
//		LIGHT_UNIFORM_LOC,								// which uniform layout
//		lightUniformBlocks[shaderProgram],				// which program buffer
//		start_index * sizeof(LightParameters),			// where to start
//		num_lights * sizeof(LightParameters));			// size to send to buffer
//
//	glBufferSubData(GL_UNIFORM_BUFFER, start_index * sizeof(LightParameters), num_lights * sizeof(LightParameters), (void *)( &( lights[start_index]) ) );
//	glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	return true;
//}

LightParameters Light::getLight(int index) const
{
	return lights[index];
}

void Light::setLight(int index, LightParameters light)
{
	lights[index] = light;
}

int Light::getLightType(int index) const
{
	return lights[index].type;
}

void Light::setLightType(int index, int type)
{
	lights[index].type = type;
}

int Light::getLightStatus(int index) const
{
	return lights[index].status;
}

void Light::turnLightOn(int index)
{
	lights[index].status = Light::STATUS_ON;
}

void Light::turnLightOff(int index)
{
	lights[index].status = Light::STATUS_OFF;
}

void Light::toggleLight(int index)
{
	if (getLightStatus(index) == Light::STATUS_ON)
	{
		turnLightOff(index);
	}
	else
	{
		turnLightOn(index);
	}
}

void Light::turnAllLightOn()
{
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		turnLightOn(i);
	}
}

void Light::turnAllLightOff()
{
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		turnLightOff(i);
	}
}

glm::vec3 Light::getLightDirection(int index) const
{
	if (lights[index].type != TYPE_DIRECTIONAL)
	{
		std::cerr << "Trying to get the position of non-directional light!" << std::endl;
		//return glm::vec3(0.0f);
	}
	return glm::vec3(lights[index].position.x, lights[index].position.y, lights[index].position.z);
}

bool Light::setAbsoluteLightDirection(int index, glm::vec3 direction)
{
	if (lights[index].type != TYPE_DIRECTIONAL)
	{
		std::cerr << "Trying to set the position of non-directional light!" << std::endl;
		return false;
	}
	lights[index].position = glm::vec4(direction,1.0f);
	return true;
}

bool Light::rotateLightDirection(int index, double p_xPos, double p_yPos, double xPos, double yPos, int window_width, int window_height)
{
	if (lights[index].type != TYPE_DIRECTIONAL)
	{
		std::cerr << "Trying to set the position of non-directional light!" << std::endl;
		return false;
	}

	glm::vec3 v1 = localTrackBallMapping(p_xPos, p_yPos, window_width, window_height);
	glm::vec3 v2 = localTrackBallMapping(xPos, yPos, window_width, window_height);
	glm::vec3 a = glm::cross(v1, v2);
	float cos_theta = glm::dot(v1, v2) / (v1.length() * v2.length());
	float theta = acos(cos_theta);

	// identity matrix 
	glm::vec3 curr_dir = glm::vec3(lights[index].position.x, lights[index].position.y, lights[index].position.z);
	glm::mat4 temp(1.0f);
	temp[3][0] = curr_dir.x * -1.0f;
	temp[3][1] = curr_dir.y * -1.0f;
	temp[3][2] = curr_dir.z * -1.0f;
	temp = glm::rotate(glm::mat4(1.0f), theta * glm::pi<float>() / 180.0f, a) * temp;
	glm::vec3 out;
	out.x = temp[3][0] * -1.0f;
	out.y = temp[3][1] * -1.0f;
	out.z = temp[3][2] * -1.0f;

	setAbsoluteLightDirection(index, out);
	return true;
}

glm::vec3 Light::getLightPosition(int index) const
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		std::cerr << "Trying to get the direction of directional light!" << std::endl;
		//return glm::vec3(0.0f);
	}
	return glm::vec3(lights[index].position.x, lights[index].position.y, lights[index].position.z);
}

bool Light::setAbsoluteLightPosition(int index, glm::vec3 position)
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		std::cerr << "Trying to set the direction of directional light!" << std::endl;
		return false;
	}
	lights[index].position = glm::vec4(position,0.0f);
	return true;
}

bool Light::setRelativeLightPosition(int index, glm::vec3 offset)
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		std::cerr << "Trying to rotate the direction of directional light!" << std::endl;
		return false;
	}
	lights[index].position += glm::vec4(offset,0.0f);
	return true;
}

glm::vec3 Light::getLightIntensity(int index) const
{
	return glm::vec3(lights[index].intensities.x, lights[index].intensities.y, lights[index].intensities.z);
}

void Light::setAbsoluteLightIntensity(int index, glm::vec3 intensity)
{
	lights[index].intensities = glm::vec4(intensity,1.0f);
}

void Light::setRelativeLightIntensity(int index, glm::vec3 offset)
{
	lights[index].intensities += glm::vec4(offset,0.0f);
}

float Light::getAttenuation(int index) const
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		return 1.0f;
	}
	return lights[index].attenuation;
}

bool Light::setAbsoluteAttenuation(int index, float attenuation)
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		std::cerr << "Setting attenuation of directional light has no effect!" << std::endl;
		return false;
	}
	lights[index].attenuation = attenuation;
	return true;
}

bool Light::setRelativeAttenuation(int index, float offset)
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		std::cerr << "Setting attenuation of directional light has no effect!" << std::endl;
		return false;
	}
	lights[index].attenuation += offset;
	return true;
}

float Light::getAmbientCoefficient(int index) const
{
	return lights[index].ambientCoefficient;
}

void Light::setAbsoluteAmbientCoefficient(int index, float ambientCoefficient)
{
	lights[index].ambientCoefficient = ambientCoefficient;
}

void Light::setRelativeAmbientCoefficient(int index, float offset)
{
	lights[index].ambientCoefficient += offset;
}

float Light::getConeAngle(int index) const
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to get the cone angle of non-spot light" << std::endl;
		return 0.0f;
	}
	return lights[index].coneAngle;
}

float Light::getConeAngleDegrees(int index) const
{
	return getConeAngle(index) * 180.0f / glm::pi<float>();
}

bool Light::setAbsoluteConeAngle(int index, float angle)
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to set the cone angle of non-spot light" << std::endl;
		return false;
	}
	lights[index].coneAngle = angle;
	return true;
}

bool Light::setRelativeConeAngle(int index, float offset)
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to set the cone angle of non-spot light" << std::endl;
		return false;
	}
	lights[index].coneAngle += offset;
	return true;
}

bool Light::setAbsoluteConeAngleDegrees(int index, float angle)
{
	return setAbsoluteConeAngle(index, angle * 180.0f / glm::pi<float>());
}

bool Light::setRelativeConeAngleDegrees(int index, float offset)
{
	return setRelativeConeAngle(index, offset * 180.0f / glm::pi<float>());
}

float Light::getConeExponent(int index) const
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to get the exponent of non-spot light" << std::endl;
		return 0.0f;
	}
	return lights[index].exponent;;
}

bool Light::setAbsoluteConeExponent(int index, float exponent)
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to set the exponent of non-spot light" << std::endl;
		return false;
	}
	lights[index].exponent = exponent;
	return true;
}

bool Light::setRelativeConeExponent(int index, float offset)
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "trying to set the exponent of non-spot light" << std::endl;
		return false;
	}
	lights[index].exponent += offset;
	return true;
}

int Light::getAttenuationType(int index) const
{
	return lights[index].attenuationType;
}

bool Light::setAttenuationType(int index, int attenuation_type)
{
	if (lights[index].type == TYPE_DIRECTIONAL)
	{
		std::cerr << "trying to set the attenuation type of directional light" << std::endl;
		return false;
	}

	lights[index].attenuationType = attenuation_type;
	return true;
}

glm::vec3 Light::getConeDirection(int index) const
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to get the cone direction of non-spot light" << std::endl;
		return glm::vec3(0.0f);
	}
	return glm::vec3(lights[index].coneDirection.x, lights[index].coneDirection.y, lights[index].coneDirection.z);
}

bool Light::setAbsoluteConeDirection(int index, glm::vec3 cone_dir)
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to set the cone direction of non-spot light" << std::endl;
		return false;
	}
	lights[index].coneDirection = glm::vec4( glm::normalize(cone_dir), 1.0f);
	return true;
}

bool Light::rotateConeDirection(int index, float angle, glm::vec3 axis)
{
	if (lights[index].type != TYPE_SPOT)
	{
		std::cerr << "Trying to set the cone direction of non-spot light" << std::endl;
		return false;
	}

	glm::vec4 temp = glm::vec4( glm::normalize(-1.0f * glm::vec3(lights[index].coneDirection)), 1.0f );
	
	// trying to rotate the spot to look at
	temp = glm::rotate(glm::mat4(1.0f), angle, axis) * temp;

	// set the cone direction
	lights[index].coneDirection = glm::vec4( glm::normalize(-1.0f * glm::vec3(temp)), 1.0f);
	return true;
}

bool Light::rotateConeDirectionDegrees(int index, float angle, glm::vec3 axis)
{
	return rotateConeDirectionDegrees(index, angle * 180.0f / glm::pi<float>(), axis);
}
