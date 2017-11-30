#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "OBJObject.h"
#include "shader.h"
#include "skybox.h"
#include "Node.h"
#include "Geometry.h"
#include "Transform.h"

struct DirectionalLight
{
	glm::vec3 light_color;
	glm::vec3 light_direction;
};

struct PointLight
{
	glm::vec3 light_color;
	glm::vec3 light_position;
	float attenuation; // attenuation constant
};

struct SpotLight
{
	glm::vec3 light_color;
	glm::vec3 light_position; // if w component=0 it’s directional
	float attenuation; // only needed for point and spotlights
	glm::vec3 cone_direction; // only needed for spotlights
	float cone_angle; // only needed for spotlights
	float exponent; // cosine exponent for how light tapers off

};


class Window
{
public:
	static int width;
	static int height;

	// cursor variables
	static double xPos, p_xPos;
	static double yPos, p_yPos;
	static float near_plane, far_plane;
	static bool left, right, l_moved, r_moved;
	static bool move_light;
	static bool special_camera;
	static bool paused;

	static std::vector< std::pair< glm::vec3, glm::vec3 > > listOfPlanes;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 localTrackBallMapping(double xpos, double ypos);
	static glm::vec3 trackball_rotate(glm::vec3 curr_pos, double p_xPos, double p_yPos, double xPos, double yPos);
	static glm::vec3 translate(glm::vec3 pos, double x, double y);

	static class World world_geod;

	static class skybox sb;

	static struct DirectionalLight dir_l;
	static struct PointLight point_l; 
	static struct SpotLight spot_l;

	static int const DIRECTIONAL = 1;
	static int const POINTLIGHT = 2;
	static int const SPOTLIGHT = 3;
	static int const CONE = 4;
	static int const SPHERE = 5;
	static int curr_light;
};



#endif
