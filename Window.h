#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Util.h"
#include "shader.h"
#include "OBJObject.h"
#include "Cube.h"
#include "Skybox.h"
#include "Rotate.h"
#include "Translate.h"
#include "Scale.h"
#include "Camera.h"
#include "Coordinate.h"
#include "Model.h"

class Window
{
public:

	static int width;
	static int height;
	 // V for view
	static void placeObject(int type, int *style);
	static Object* testForCollision(int *face);
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButton_callback(GLFWwindow* window, int button, int action, int mods);
	static void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mousePos_callback(GLFWwindow* window, double xpos, double ypos);
	static void loadAllShader();
};

#endif
