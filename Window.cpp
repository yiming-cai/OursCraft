#include "window.h"

// define the static variables declared in header
const char* window_title = "GLFW Starter Project";
World Window::world_geod;
GLint shaderProgram;
double Window::xPos = 0, Window::p_yPos, Window::p_xPos, Window::yPos = 0;
bool Window::left = false, Window::right = false, Window::l_moved = true, Window::r_moved = true;
bool Window::move_light = false;
float Window::near_plane = 0.1f, Window::far_plane = 300.0f;
DirectionalLight Window::dir_l;
PointLight Window::point_l;
SpotLight Window::spot_l;
int Window::curr_light = Window::DIRECTIONAL;
bool Window::special_camera = false;
bool Window::paused = false;
std::vector< std::pair< glm::vec3, glm::vec3 > > Window::listOfPlanes;
skybox Window::sb;
//Cube * cube;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 100.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;


void Window::initialize_objects()
{
	// settings on the assignment
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	std::vector<std::string> face_strings = 
	{   "../skybox_assets/right.jpg", 
		"../skybox_assets/left.jpg", 
		"../skybox_assets/top.jpg",
		"../skybox_assets/bottom.jpg",
		"../skybox_assets/front.jpg",
		"../skybox_assets/back.jpg" };

	// Create a new skybox
	sb = skybox(face_strings);
	sb.loadCubemap();
	sb.initialize();

	std::cerr << "error here?" << std::endl;
	// Create a new Transform class
	world_geod = World();

	//ArmyOfRobot * r = new ArmyOfRobot();
	//world_geod.addChild(r);
	//r->M = glm::mat4(1.0f);

	ConnectedCurves * curve = new ConnectedCurves();
	curve->M = glm::mat4(1.0f);
	world_geod.addChild(curve);

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	dir_l = {
		glm::vec3(0.95f, 0.95f, 0.95f), // light_color or intensity
		glm::normalize(glm::vec3(0.0f, -1.0f, -0.4f)) // light_direction
	};

	//point_l = {
	//	glm::vec3(0.80f, 0.80f, 0.80f), // light_color
	//	glm::vec3(0.0f, 3.0f, 0.0f),	//light_position
	//	0.15f	// attenuation
	//};

	//spot_l = { 
	//	glm::vec3(0.80f, 0.80f, 0.80f), // light_color
	//	glm::vec3(0.0f, 6.0f, 0.0f), // light_position
	//	0.05f, // attenuation
	//	glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), // cone direction
	//	20.0f * glm::pi<float>() / 180.0f, // cone angle = 10 degress -> radians
	//	2.0f // spot exponent
	//};

	//listOfPlanes.push_back(std::make_pair(glm::vec3(0, 0, far_plane), glm::vec3(0, 0, 1.0f)));	// far plane
	//listOfPlanes.push_back(std::make_pair(glm::vec3(0, 0, near_plane), glm::vec3(0, 0, -1.0f)));	// near plane
	//listOfPlanes.push_back(std::make_pair(glm::vec3(0, -1.0f, 0), glm::vec3(0, -1.0f, 0)));	// base plane
	//listOfPlanes.push_back(std::make_pair(glm::vec3(0, 1.0f, 0), glm::vec3(0, 1.0f, 0)));	// top plane
	//listOfPlanes.push_back(std::make_pair(glm::vec3(1.0f, 0, 0), glm::vec3(1.0f, 0, 0)));	// right plane
	//listOfPlanes.push_back(std::make_pair(glm::vec3(-1.0f, 0, 0), glm::vec3(-1.0f, 0, 0)));	// left plane
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	// TODO: delete all the dynamic nodes later

	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, near_plane, far_plane);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	static int released = 0;
	static bool just_switched = false;
	static glm::vec3 prev_cam_pos, prev_cam_look_at, prev_cam_up;

	if (special_camera && !just_switched)
	{
		just_switched = true;
		prev_cam_pos = cam_pos;
		prev_cam_look_at = cam_look_at;
		prev_cam_up = cam_up;
	}
	
	if (special_camera && just_switched)
	{
		world_geod.changeCamera(V, cam_pos, cam_look_at, cam_up);
	}
	else if (!special_camera && just_switched)
	{
		just_switched = false;
		cam_pos = prev_cam_pos;
		cam_look_at = prev_cam_look_at;
		cam_up = prev_cam_up;
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}

	if (!paused)
		world_geod.update();

	// when we are not moving the lights
	if (left && !l_moved)
	{
		if (released == 0)
		{
			if (!world_geod.changeCPPosition(width, height, p_xPos, p_yPos, xPos, yPos, P, V))
			{
				cam_pos = trackball_rotate(cam_pos - cam_look_at, p_xPos, p_yPos, xPos, yPos);
				cam_pos += cam_look_at;
				V = glm::lookAt(cam_pos, cam_look_at, cam_up);
				released = 2;
			}

			else
			{
				released = 1;
			}

		}
		else if (released == 1)
		{
			world_geod.continueChangePosition(width, height, p_xPos, p_yPos, xPos, yPos, P, V);
		}
		else if (released == 2)
		{
			cam_pos = trackball_rotate(cam_pos - cam_look_at, p_xPos, p_yPos, xPos, yPos);
			cam_pos += cam_look_at;
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			released = 2;
		}

		l_moved = true;

	}
	else if (!left)
	{
		released = 0;
	}

	if (right && !r_moved)
	{
		// TODO: do smth when right mouse button is used
		cam_pos = translate(cam_pos, xPos - p_xPos, yPos - p_yPos);
		cam_look_at = translate(cam_look_at, xPos - p_xPos, yPos - p_yPos);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		r_moved = true;
	}
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	
	// Render the cube
	//cube->draw(shaderProgram);

	GLint loc;

	loc = glGetUniformLocation(shaderProgram, "resolution");
	glUniform2f(loc, (float) Window::width, (float) Window::height);

	loc = glGetUniformLocation(shaderProgram, "cam_pos");
	glUniform3f(loc, cam_pos.x, cam_pos.y, cam_pos.z);

	loc = glGetUniformLocation(shaderProgram, "dir_l.light_color");
	glUniform3f(loc, dir_l.light_color.r, dir_l.light_color.g, dir_l.light_color.b);

	loc = glGetUniformLocation(shaderProgram, "dir_l.light_direction");
	glUniform3f(loc, dir_l.light_direction.x, dir_l.light_direction.y, dir_l.light_direction.z);

	//loc = glGetUniformLocation(shaderProgram, "point_l.light_color");
	//glUniform3f(loc, point_l.light_color.r, point_l.light_color.g, point_l.light_color.b);

	//loc = glGetUniformLocation(shaderProgram, "point_l.light_position");
	//glUniform3f(loc, point_l.light_position.x, point_l.light_position.y, point_l.light_position.z);

	//loc = glGetUniformLocation(shaderProgram, "point_l.attenuation");
	//glUniform1f(loc, point_l.attenuation);

	//loc = glGetUniformLocation(shaderProgram, "spot_l.light_color");
	//glUniform3f(loc, spot_l.light_color.r, spot_l.light_color.g, spot_l.light_color.b);

	//loc = glGetUniformLocation(shaderProgram, "spot_l.light_position");
	//glUniform3f(loc, spot_l.light_position.x, spot_l.light_position.y, spot_l.light_position.z);

	//loc = glGetUniformLocation(shaderProgram, "spot_l.attenuation");
	//glUniform1f(loc, spot_l.attenuation);

	//loc = glGetUniformLocation(shaderProgram, "spot_l.cone_direction");
	//glUniform3f(loc, spot_l.cone_direction.x, spot_l.cone_direction.y, spot_l.cone_direction.z);

	//loc = glGetUniformLocation(shaderProgram, "spot_l.cone_angle");
	//glUniform1f(loc, spot_l.cone_angle);

	//loc = glGetUniformLocation(shaderProgram, "spot_l.exponent");
	//glUniform1f(loc, spot_l.exponent);

	//change the light mode if necessary
	loc = glGetUniformLocation(shaderProgram, "light_mode");

	// display the skybox first (not optimized)
	sb.display(shaderProgram);

	//if (curr_light == SPOTLIGHT && current->i_normal_mode == 0)
	//{
	//	curr_light = CONE;
	//	glUniform1i(loc, curr_light);
	//	cone->draw(shaderProgram);
	//	curr_light = SPOTLIGHT;
	//}

	//if (curr_light == POINTLIGHT && current->i_normal_mode == 0)
	//{
	//	curr_light = SPHERE;
	//	glUniform1i(loc, curr_light);
	//	sphere->draw(shaderProgram);
	//	curr_light = POINTLIGHT;
	//}

	glUniform1i(loc, curr_light);

	// far plane
	//listOfPlanes[0].first = cam_pos + near_plane * glm::normalize(cam_look_at - cam_pos);
	//listOfPlanes[0].second = glm::normalize(cam_pos - cam_look_at);

	//// near plane
	//listOfPlanes[1].first = cam_pos + far_plane * glm::normalize(cam_look_at - cam_pos);
	//listOfPlanes[1].second = glm::normalize(cam_look_at - cam_pos);

	//world_geod.cull(listOfPlanes, glm::mat4(1.0f), Window::P,  Window::V);
	world_geod.draw(shaderProgram, glm::mat4(1.0f));

	//if (draw_sphere)
	//{
	//	loc = glGetUniformLocation(shaderProgram, "draw_sphere");
	//	glUniform1i(loc, 1);
	//	world_geod.drawBoundingSphere(shaderProgram, glm::mat4(1.0f));
	//	glUniform1i(loc, 0);
	//}

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);

}

// Cursor position callback function
void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	l_moved = false;
	r_moved = false;
	p_xPos = xPos;
	p_yPos = yPos;
	xPos = xpos;
	yPos = ypos;
}

// Mouse button event button
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		left = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		left = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		right = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		right = false;
	}
}

// Mouse scroll event button, for moving the model in and out
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!move_light)
	{
		if (glm::length(cam_pos - cam_look_at) > 0.7f || yoffset > 0.5)
		{
			cam_pos += glm::normalize(cam_pos - cam_look_at) * 0.6f * (float) ( (float)yoffset > 0.5 ? 1 : -1 );
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
	}
	else
	{
		if (curr_light == POINTLIGHT)
		{
			//// move 0.5 units 
			//if (glm::length(point_l.light_position) > 0.5 || yoffset > 0.5)
			//{
			//	point_l.light_position += glm::normalize(point_l.light_position) * 0.5f * (float)yoffset;
			//	sphere->setPosition(point_l.light_position);
			//}
			//else
			//{
			//	point_l.light_position = glm::normalize(point_l.light_position) * 0.1f;
			//	sphere->setPosition(point_l.light_position);
			//}
		}
		else if (curr_light == SPOTLIGHT)
		{
			//// move 0.5 units 
			//if (glm::length(spot_l.light_position) > 0.5 || yoffset > 0.5)
			//{
			//	spot_l.light_position += glm::normalize(spot_l.light_position) * 0.5f * (float)yoffset;
			//	cone->setPosition(spot_l.light_position);
			//}
			//else
			//{
			//	spot_l.light_position = glm::normalize(spot_l.light_position) * 0.1f;
			//	cone->setPosition(spot_l.light_position);
			//}
		}
	}
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// 1,2,3 buttons for selecting lights and change to light manipulation mode
		//else if (key == GLFW_KEY_1)
		//{
		//	move_light = true;
		//	curr_light = DIRECTIONAL;
		//}
		//else if (key == GLFW_KEY_2)
		//{
		//	move_light = true;
		//	curr_light = POINTLIGHT;
		//}
		//else if (key == GLFW_KEY_3)
		//{
		//	move_light = true;
		//	curr_light = SPOTLIGHT;
		//}

		// 0 button for deselecting lights and change to model manipulation mode
		else if (key == GLFW_KEY_0)
		{
			move_light = false;
		}
		else if (key == GLFW_KEY_P)
		{
			((ConnectedCurves *)world_geod.children[0])->printVertices();
		}

		//else if (key == GLFW_KEY_S)
		//{
		//	// upper case
		//	ArmyOfRobot * r = (ArmyOfRobot *)world_geod.children[0];
		//	if ((mods & GLFW_MOD_SHIFT) != 0)
		//	{
		//		r->M = glm::scale(r->M, glm::vec3(1.25, 1.25, 1.25));
		//	}
		//	else
		//	{
		//		r->M = glm::scale(r->M, glm::vec3(0.80, 0.80, 0.80));
		//	}
		//}

		//else if (key == GLFW_KEY_X)
		//{
		//	ArmyOfRobot * r = (ArmyOfRobot *) world_geod.children[0];
		//	if ((mods & GLFW_MOD_SHIFT) != 0)
		//	{
		//		r->M = glm::rotate(r->M, 5.0f * glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//	}
		//	else
		//	{
		//		r->M = glm::rotate(r->M, -5.0f * glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//	}
		//}
		//else if (key == GLFW_KEY_Y)
		//{
		//	ArmyOfRobot * r = (ArmyOfRobot *)world_geod.children[0];
		//	if ((mods & GLFW_MOD_SHIFT) != 0)
		//	{
		//		r->M = glm::rotate(r->M, 5.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//	}
		//	else
		//	{
		//		r->M = glm::rotate(r->M, -5.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//	}
		//}
		//else if (key == GLFW_KEY_Z)
		//{
		//	ArmyOfRobot * r = (ArmyOfRobot *)world_geod.children[0];
		//	if ((mods & GLFW_MOD_SHIFT) != 0)
		//	{
		//		r->M = glm::rotate(r->M, 5.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//	}
		//	else
		//	{
		//		r->M = glm::rotate(r->M, -5.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//	}
		//}
		else if (key == GLFW_KEY_SPACE)
		{
			paused = !paused;
		}
		else if (key == GLFW_KEY_R)
		{
			world_geod.reset();
		}
		else if (key == GLFW_KEY_S)
		{
			special_camera = !special_camera;
		}
		else if (key == GLFW_KEY_F)
		{
			world_geod.turnFrictionOn();
		}
	}
}


glm::vec3 Window::trackball_rotate(glm::vec3 curr_dir, double p_xPos, double p_yPos, double xPos, double yPos)
{
	glm::vec3 v1 = localTrackBallMapping(p_xPos, p_yPos);
	glm::vec3 v2 = localTrackBallMapping(xPos, yPos);
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
	return out;
}

glm::vec3 Window::translate(glm::vec3 vec, double x, double y)
{
	glm::vec3 temp((float)x, (float)y, 0);
	float dim = (float)std::min(Window::width, Window::height);
	temp.x /= (dim / 20.0f);
	temp.y /= -1.0f * (dim / 20.0f);
	return glm::vec3( glm::translate(glm::mat4(1.0f), temp) * glm::vec4(vec, 1.0f) );
}

glm::vec3 Window::localTrackBallMapping(double xpos, double ypos)
{
	glm::vec3 v;    // Vector v is the synthesized 3D position of the mouse location on the trackball
	float d;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
	v.x = (2.0f* (float)xpos - (float)Window::width) / (float)Window::width;   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
	v.y = ((float)Window::height - 2.0f* (float)ypos) / (float)Window::height;   // this does the equivalent to the above for the mouse Y position
	v.z = 0.0f;   // initially the mouse z position is set to zero, but this will change below
	d = glm::length(v);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
	d = (d<1.0f) ? d : 1.0f;   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
	v.z = sqrtf(1.001f - d*d);  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
	glm::normalize(v);
	return v;  // return the mouse location on the surface of the trackball
}