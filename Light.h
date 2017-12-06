#pragma once

#include "Util.h"
#include <stdlib.h>     /* srand, rand */

const static LightParameters DIRECTIONAL_PRESET =
{
	glm::normalize(glm::vec3(-1.0f,-1.0f,-1.0f)),	// light direction
	glm::vec3(1.0f,1.0f,1.0f),						// light intensity
	glm::vec3(0.0f),								// cone direction
	0.0f,											// light attenuation
	1.0f,											// ambient light coefficient
	0.0f,											// cone angle
	0.0f,											// spot exponent
	0,												// light type, set to directional
	0,												// attenuation type, set to constant
	0,												// light status, set to off
};

const static LightParameters POINTLIGHT_PRESET =
{
	glm::vec3(0, 5.0f, 0),							// light direction
	glm::vec3(1.0f),								// light intensity
	glm::vec3(0.0f),								// cone direction
	0.25f,											// light attenuation
	0.25f,											// ambient light coefficient
	0.0f,											// cone angle
	0.0f,											// spot exponent
	1,												// light type, set to point light
	1,												// attenuation type, set to linear
	0,												// light status, set to off
};

const static LightParameters SPOTLIGHT_PRESET =
{
	glm::vec3(0, 5.0f, 0),							// light direction
	glm::vec3(1.0f),								// light intensity
	glm::vec3(0, -1.0f, 0),							// cone direction
	0.25f,											// light attenuation
	0.25f,											// ambient light coefficient
	1.0f,											// cone angle, set to 1 radian (or 180/pi degrees)
	32.0f,											// spot exponent
	2,												// light type, set to spot light
	1,												// attenuation type, set to linear
	0,												// light status, set to off
};

class Light
{
private:

	glm::vec3 localTrackBallMapping(double xpos, double ypos, int width, int height);

	// 
	//struct LightParameters = {
	//
	//glm::vec3 position;       : also used as direction for directional light
	//glm::vec3 intensities;	: a.k.a the color of the light
	//glm::vec3 coneDirection;  : only needed for spotlights

	//float attenuation;        : only needed for point and spotlights
	//float ambientCoefficient; : how strong the light ambience should be... 0 if there's no ambience (background reflection) at all
	//float coneAngle;          : only needed for spotlights
	//float exponent;           : cosine exponent for how light tapers off
	//int type;                 : specify the type of the light (directional, spotlight, point light)
	//int attenuationType;      : specify the type of attenuation to use
	//int status;			    : 0 for turning off the light, 1 for turning on the light
	//
	//}

	// A fixed size array of lights
	std::vector<LightParameters> lights;

	// a fixed seed for randomly initializing lights
	int srand_seed = 15;

	const static int LIGHT_UNIFORM_LOC = 5;

public:

	const static int TYPE_DIRECTIONAL = 0;
	const static int TYPE_POINT = 1;
	const static int TYPE_SPOT = 2;

	const static int ATTENUATION_CONSTANT = 0;
	const static int ATTENUATION_LINEAR = 1;
	const static int ATTENUATION_QUADRATIC = 2;

	const static int STATUS_ON = 0;
	const static int STATUS_OFF = 1;

	const static unsigned int NUM_LIGHTS = 16;

	Light();
	~Light();

	// set the light at 'index' to become one of the preset
	void set_preset_directional(int index);
	void set_preset_point(int index);
	void set_preset_spot(int index);
	
	/* -------- !call this function to update lights in shaders! --------- */
	void updateShader();
	/* ------------------------------------------------------------------- */


	/* -------- try not to use these --------------- */
	// get the light at the index
	LightParameters getLight(int index) const;

	// set the light at the index
	void setLight(int index, LightParameters light);
	/* --------------------------------------------- */


	/* -------- deal with the type of lights -------- */

	// return the type of light
	// preferably, if you want to check light type, use:
	//		if (getLightType(i) == Light::DIRECTIONAL) { // do something }
	int getLightType(int index);

	// set the type of light
	void setLightType(int index, int type);
	/* ---------------------------------------------- */


	/* ------- deal with toggling the lights on and off ------------- */
	// check if light is turned on or off
	int getLightStatus(int index);

	void turnLightOn(int index);

	void turnLightOff(int index);

	void toggleLight(int index);
	/* -------------------------------------------------------------- */


	/* ---------- deal with the direction of the lights, directional lights only ------------- */

	// get the light direction, will return all 0s if light is not directional
	glm::vec3 getLightDirection(int index);

	// set the absolute light direction
	void setAbsoluteLightDirection(int index, glm::vec3 direction);

	// rotate the light based on mouse movements (must pass in the pair of mouse position previously and
	// the pair of mouse position currently, as well as the window width and height
	void rotateLightDirection(int index, glm::vec3 curr_dir, double p_xPos, double p_yPos, double xPos, double yPos, int window_width, int window_height);
	/* ------------------------------------------------------------ */


	/* ---------- deal with the position of the lights, non-directional lights only ------------- */

	// get the light position, will return all 0s if light is directional
	glm::vec3 getLightPosition(int index);

	// set the light position, in world coordinates, will do nothing if light is directional
	void setAbsoluteLightPosition(int index, glm::vec3 position);

	// set the light position, relative to current position of the light, will do nothing if light is directional
	void setRelativeLightPosition(int index, glm::vec3 offset);
	/* -------------------------------------------------------------------------------------------*/

	
	/* ---------- deal with light intensities --------------------------------------------*/

	// get the light intensity
	glm::vec3 getLightIntensity(int index);

	// set the absolute light intensity 
	void setAbsoluteLightIntensity(int index, glm::vec3 intensity);

	// set the light intensity relative to current intensity
	void setRelativeLightIntensity(int index, glm::vec3 offset);
	/* ---------------------------------------------------------------------------------- */
	

	/* --------------- deal with attenuation -------------------------------------------- */
	// get the light attenuation value
	float getAttenuation(int index);

	// set the light attenuation value
	void setAbsoluteAttenuation(int index, float attenuation);

	// set the light attenuation value (increase or decrease) relative to current value
	void setRelativeAttenuation(int index, float offset);
	/* ----------------------------------------------------------------------------------- */


	/* --------------- deal with ambientCoefficient --------------------------------------- */
	// get the light ambientCoefficient value
	float getAmbientCoefficient(int index);

	// set the light ambientCoefficient value
	void setAbsoluteAmbientCoefficient(int index, float ambientCoefficient);

	// set the light ambientCoefficient value (increase or decrease) relative to current value
	void setRelativeAmbientCoefficient(int index, float offset);
	/* ----------------------------------------------------------------------------------- */


	/* --------------- deal with cone angle ---------------------------------------------- */
	// get the cone angle (width), in radians
	float getConeAngle(int index);

	// get the cone angle (width), in degrees
	float getConeAngleDegrees(int index);

	// set the absolute cone angle, in radians
	void setAbsoluteConeAngle(int index, float angle);

	// set the cone angle relative to current angle, in radians
	void setRelativeConeAngle(int index, float offset);

	// set the absolute cone angle, in degrees
	void setAbsoluteConeAngleDegrees(int index, float angle);

	// set the cone angle relative to current angle, in degrees
	void setRelativeConeAngleDegrees(int index, float offset);
	/* ----------------------------------------------------------------------------------- */


	/* ----------- deal with spotlight wear-off exponent --------------------------------- */
	// get the cone exponent
	float getConeExponent(int index);

	// set the absolute value of the cone exponent
	void setAbsoluteConeExponent(int index, float exponent);

	// set the value of the cone exponent relative to current value
	void setRelativeConeExponent(int index, float exponent);
	/* ------------------------------------------------------------------------------------ */


	/* ------------ deal with the attenuation type ---------------------------------------- */
	int getAttenuationType(int index);

	void setAttenuationType(int index, int attenuation_type);
	/* ------------------------------------------------------------------------------------ */


	/* --------------- deal with cone direction ------------------------------------------- */
	glm::vec3 getConeDirection(int index);

	void setAbsoluteConeDirection(int index, glm::vec3 cone_dir);

	void rotateConeDirection(int index, float angle, glm::vec3 axis);
	/* ------------------------------------------------------------------------------------ */
};

