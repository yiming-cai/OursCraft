#pragma once

#include "Util.h"
#include "shader.h"
#include "Cube.h"
#include "Skybox.h"
#include "Rotate.h"
#include "Translate.h"
#include "Scale.h"
#include "Camera.h"
#include "Coordinate.h"
#include "Model.h"
#include "SimplePointer.h"
#include "OBJObject.h"
#include "Light.h"
#include "LightDisplay.h"
#include "Water.h"
#include "Fog.h"
#include "Sound.h"
#include "ParticleGenerator.h"
#include <iostream>
#include <sstream>
#include <fstream>

class Window
{
public:

	const static int FILE_INDEX_ID = 0;
	const static int FILE_INDEX_POS_X = 1;
	const static int FILE_INDEX_POS_Y = 2;
	const static int FILE_INDEX_POS_Z = 3;
	const static int FILE_INDEX_SIZE = 4;
	const static int FILE_INDEX_TEXTUREID = 5;
	const static int FILE_INDEX_COLOR_X = 6;
	const static int FILE_INDEX_COLOR_Y = 7;
	const static int FILE_INDEX_COLOR_Z = 8;

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

	static bool loadFromFile(std::string filename);
};

