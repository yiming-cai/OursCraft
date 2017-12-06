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

				break;

			case Light::TYPE_POINT:
				set_preset_point(i);

				// randomly sets the light position
				setAbsoluteLightPosition(
					i,
					glm::vec3
					(
						(float)(rand() % 100) / 10.0f,
						(float)(rand() % 100) / 10.0f,
						(float)(rand() % 100) / 10.0f
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

				break;

			case Light::TYPE_SPOT:
				set_preset_spot(i);

				// randomly sets the light position
				setAbsoluteLightPosition(
					i,
					glm::vec3
					(
					(float)(rand() % 100) / 10.0f,
						(float)(rand() % 100) / 10.0f,
						(float)(rand() % 100) / 10.0f
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

				// make it point to the origin, or center of the map
				setAbsoluteConeDirection(i, glm::vec3(0.0f) - getConeDirection(i));

				// make it randomly wide, as wide as up to 120 degrees
				setAbsoluteConeAngleDegrees(i, (float)(rand()%120) );
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

LightParameters Light::getLight(int index) const
{
	return lights[index];
}

void Light::setLight(int index, LightParameters light)
{
	lights[index] = light;
}

int Light::getLightType(int index)
{
	return lights[index].type;
}

void Light::setLightType(int index, int type)
{
}

int Light::getLightStatus(int index)
{
	return 0;
}

void Light::turnLightOn(int index)
{
}

void Light::turnLightOff(int index)
{
}

void Light::toggleLight(int index)
{
}

glm::vec3 Light::getLightDirection(int index)
{
	return glm::vec3();
}

void Light::setAbsoluteLightDirection(int index, glm::vec3 direction)
{
}

void Light::rotateLightDirection(int index, glm::vec3 curr_dir, double p_xPos, double p_yPos, double xPos, double yPos, int window_width, int window_height)
{
	glm::vec3 v1 = localTrackBallMapping(p_xPos, p_yPos, window_width, window_height);
	glm::vec3 v2 = localTrackBallMapping(xPos, yPos, window_width, window_height);
	glm::vec3 a = glm::cross(v1, v2);
	float cos_theta = glm::dot(v1, v2) / (v1.length() * v2.length());
	float theta = acos(cos_theta);

	// identity matrix 
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
}

glm::vec3 Light::getLightPosition(int index)
{
	return glm::vec3();
}

void Light::setAbsoluteLightPosition(int index, glm::vec3 position)
{
}

void Light::setRelativeLightPosition(int index, glm::vec3 offset)
{
}

glm::vec3 Light::getLightIntensity(int index)
{
	return glm::vec3();
}

void Light::setAbsoluteLightIntensity(int index, glm::vec3 intensity)
{
}

void Light::setRelativeLightIntensity(int index, glm::vec3 offset)
{
}

float Light::getAttenuation(int index)
{
	return 0.0f;
}

void Light::setAbsoluteAttenuation(int index, float attenuation)
{
}

void Light::setRelativeAttenuation(int index, float offset)
{
}

float Light::getAmbientCoefficient(int index)
{
	return 0.0f;
}

void Light::setAbsoluteAmbientCoefficient(int index, float ambientCoefficient)
{
}

void Light::setRelativeAmbientCoefficient(int index, float offset)
{
}

float Light::getConeAngle(int index)
{
	return 0.0f;
}

float Light::getConeAngleDegrees(int index)
{
	return 0.0f;
}

void Light::setAbsoluteConeAngle(int index, float angle)
{
}

void Light::setRelativeConeAngle(int index, float offset)
{
}

void Light::setAbsoluteConeAngleDegrees(int index, float angle)
{
}

void Light::setRelativeConeAngleDegrees(int index, float offset)
{
}

float Light::getConeExponent(int index)
{
	return 0.0f;
}

void Light::setAbsoluteConeExponent(int index, float exponent)
{
}

void Light::setRelativeConeExponent(int index, float exponent)
{
}

int Light::getAttenuationType(int index)
{
	return 0;
}

void Light::setAttenuationType(int index, int attenuation_type)
{
}

glm::vec3 Light::getConeDirection(int index)
{
	return glm::vec3();
}

void Light::setAbsoluteConeDirection(int index, glm::vec3 cone_dir)
{
}

void Light::rotateConeDirection(int index, float angle, glm::vec3 axis)
{
}
