#include "Window.h"


extern glm::mat4 P;
extern glm::mat4 V;
extern GLuint Shader_Geometry; 
extern GLuint Shader_Skybox;
extern GLuint Shader_Coordinate;
extern GLuint Shader_Model;
extern GLuint Shader_SimplePointer;


std::vector<Object *> objectList;
std::vector<Camera *> cameraList;


int pickType = 0;
int pickStyle = 0;
int idCount = 0;
int leftMousePressed,rightMousePressed;
int pickObjectFace;
int showCoordinate;
int goRight, goLeft, goUp, goDown, goForward, goBackward;
float mouseX, mouseY;
glm::vec3 ray_dir;
Object* pickObject = NULL;

Cube *cube;
Skybox *skybox;
Camera *currentCam;
Coordinate *coordinate;
SimplePointer *centerRouter;

std::vector<std::string> faces
{
	"../assets/right.jpg",
	"../assets/left.jpg",
	"../assets/top.jpg",
	"../assets/bottom.jpg",
	"../assets/back.jpg",
	"../assets/front.jpg"
};


const char* window_title = "GLFW Starter Project";

// ------------ FOR TESTING ONLY ------------
Model * model;
Light lights;
//-------------------------------------------

Model * model1;
Model * model2;

int keyPressed;
int shiftPressed;
// On some systems you need to change this to the absolute path

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 10.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;



void Window::placeObject(int type, int *style)
{
	if (type == 0) return;
	if (pickObject != NULL) {
		glm::vec3 npos = pickObject->getObjectBiasPosition(pickObjectFace);
		switch (type) {
		case 1:
			if (*style >= CUBE_TEXTURE_NUM) *style = 0;
			if (*style < 0) *style = CUBE_TEXTURE_NUM - 1;
			cube = new Cube(idCount++, 1, *style);
			cube->setPosition(npos.x, npos.y, npos.z);
			objectList.push_back(cube);
			break;
		case 2:break;
		default:break;
		}
		
	}
}

Object* Window::testForCollision(int *face) {
	Object *obj = NULL;
	float value = 999999;
	int fff = 0;
	for (int i = 0; i < objectList.size(); ++i) {
		//objectList[i]->getId();
		float j = objectList[i]->getCollisionValue(currentCam->camera_pos, ray_dir,&fff);
		if (j > 0 && j < value) {
			*face = fff;
			value = j;
			obj = objectList[i];
		}
	}
	if (obj) {
		obj->setSelected();
	}
	return obj;
}

void Window::loadAllShader() {
	Shader_Geometry = LoadShaders(GEOMETRY_VERTEX_SHADER_PATH, GEOMETRY_FRAGMENT_SHADER_PATH);
	Shader_Skybox = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
	Shader_Coordinate = LoadShaders(COORDINATE_VERTEX_SHADER_PATH, COORDINATE_FRAGMENT_SHADER_PATH);
	Shader_Model = LoadShaders(MODEL_VERTEX_SHADER_PATH, MODEL_FRAGMENT_SHADER_PATH);
	Shader_SimplePointer = LoadShaders(SIMPLE_POINTER_VERTEX_SHADER_PATH, SIMPLE_POINTER_FRAGMENT_SHADER_PATH);
	
}

void Window::initialize_objects()
{
	leftMousePressed = 0;
	mouseX = mouseY = MOUSEPOS_INIT_VALUE;
	goRight = goLeft = goUp = goDown = goForward = goBackward = 0;
	showCoordinate = 0;
	loadAllShader();
	//	printf("LoadShaders Finished!2 %d\n", Shader_Geometry);
	for (int i = -3; i <= 3; ++i)
		for (int j = -3; j <= 3; ++j) {
			//cube = new Cube(idCount++, 1, glm::vec3(0.94, 1, 1));
			cube = new Cube(idCount++, 1, 3);
			cube->setPosition(i, GROUND_LEVEL - 1, j);
			objectList.push_back(cube);
		}
	skybox = new Skybox(idCount++, 1000, &faces);
	currentCam = new Camera(idCount++, glm::vec3(0, 1, 0), glm::vec3(0, 1, -1), glm::vec3(0, 1, 0));
	coordinate = new Coordinate(idCount++, 100);
	centerRouter = new SimplePointer(idCount++, 0, 0, glm::vec3(1, 0, 0));
	ray_dir = glm::vec3(0, 0.5, -1);
	printf("Init All Done\n PLEASE TYPE 1-4 to select Object, and use I O to select Texture");

	// Enables backface culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	// ------------------FOR TESTING ONLY ---------------------
	// Create a test model
	model = new Model("../ferrari/polygons.lwo");
	model->setCamera(currentCam);
	model->initShader(Shader_Model);
	model->centerAndScale(1.0f);

	// create a test light
	lights = Light();
	lights.randInit();
	lights.initializeShader(Shader_Model);
	lights.updateShader(Shader_Model);
	// --------------------------------------------------------------
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
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
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 2000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	if (goForward) currentCam->cameraMove(0);
	if (goBackward) currentCam->cameraMove(1);
	if (goRight) currentCam->cameraMove(2);
	if (goLeft) currentCam->cameraMove(3);
	if (goUp) currentCam->cameraMove(4);
	if (goDown) currentCam->cameraMove(5);

	// update Camera
	V = currentCam->getCameraV();

	// init intersection
	pickObject = testForCollision(&pickObjectFace);
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw coordinate
	if (showCoordinate) coordinate->draw(glm::mat4(1.0f));

	//draw a cursor
	centerRouter->draw(glm::mat4(1.0f));

	//draw skybox
	skybox->draw(glm::mat4(1.0f));

	// draw object
	for (int i = 0; i < objectList.size();++i)
		objectList[i]->draw(glm::mat4(1.0f));
	

	// test draw model
	for (int j = 0; j < 6; j++) {
		model->draw(glm::translate(glm::mat4(1.0f), { 0,1,j })*glm::mat4(1.0f), Shader_Model);
	}

	//model1->draw(glm::mat4(1.0f));

	glfwPollEvents();

	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		rightMousePressed = 1;
		if (pickObject != NULL) {
			for (int i = 0; i < objectList.size(); ++i) {
				if (objectList[i]->getId() == pickObject->getId()) {
					objectList.erase(objectList.begin() + i);
					break;
				}
			}
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		leftMousePressed = 1;
		placeObject(pickType, &pickStyle);
		
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		rightMousePressed = 0;
		//mouseX = mouseY = MOUSEPOS_INIT_VALUE;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		leftMousePressed = 0;
		mouseX = mouseY = MOUSEPOS_INIT_VALUE;
	}
}

void Window::mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}
/*
float compute_angle(float x1,float y1,float x2,float y2)  
{
	float r = MAX(Window::width, Window::height);
	r /= 2;
	x1 -= sphereX;
	x2 -= sphereX;
	y1 -= sphereY;
	y2 -= sphereY;
	float len1 = x1 * x1 + y1 * y1;
	float len2 = x2 * x2 + y2 * y2;
	len1 = len1 > r * r - 1 ? r * r : len1;
	len2 = len2 > r * r - 1 ? r * r : len2;

	float z1 = sqrtf(r * r - len1);
	float z2 = sqrtf(r * r - len2);
	glm::vec3 v1(x1, y1, z1);
	glm::vec3 v2(x2, y2, z2);
	v1 = glm::normalize(v1);
	v2 = glm::normalize(v2);
	
	//printf("%f %f %f %f %f %f\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
	return glm::acos(glm::dot(v1,v2)) * 180.0f / glm::pi<float>();
}

glm::vec3 compute_axis(float x1, float y1, float x2, float y2) {
	float r = MAX(Window::width, Window::height);
	r /= 2;
	x1 -= sphereX;
	x2 -= sphereX;
	y1 -= sphereY;
	y2 -= sphereY;
	float len1 = x1 * x1 + y1 * y1;
	float len2 = x2 * x2 + y2 * y2;
	len1 = len1 > r * r - 1 ? r * r : len1;
	len2 = len2 > r * r - 1 ? r * r : len2;

	float z1 = sqrtf(r * r - len1);
	float z2 = sqrtf(r * r - len2);
	//printf("%f %f\n", z1, z2);
	glm::vec3 v1(x1, y1, z1);
	glm::vec3 v2(x2, y2, z2);
	v1 = glm::normalize(v1);
	v2 = glm::normalize(v2);
	glm::vec3 re = glm::cross(v1, v2);
	//printf("%f %f %f %f %f %f\n", v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
	return re;
}
*/

void Window::mousePos_callback(GLFWwindow* window, double xpos, double ypos) {

	if (leftMousePressed == 0) {
		if (mouseX == MOUSEPOS_INIT_VALUE) {
			mouseX = xpos;
			mouseY = ypos;
		}
		else {
			glm::vec2 delta = glm::vec2((xpos - mouseX) * 7 / Window::width, (mouseY - ypos) * 7 / Window::height);
			//printf("%f %f\n", delta.x, delta.y);
			currentCam->cameraRotate(delta);
			mouseX = xpos;
			mouseY = ypos;
			float x = 0; // (2.0f * xpos) / Window::width - 1.0f;
			float y = 0; // 1.0f - (2.0f * ypos) / Window::height;
			float z = 1.0f;
			//printf("%f %f %f\n", x, y, z);
			glm::vec4 ray = glm::vec4(x, y, z, 1.0);
			glm::vec4 ray_eye = glm::inverse(P) * ray;
			glm::vec4 ray_world = glm::inverse(V) * ray_eye;
			//printf("%f %f %f %f\n", ray_world.x / ray_world.w, ray_world.y / ray_world.w, ray_world.z / ray_world.w, ray_world.w);
			ray_dir = glm::normalize(glm::vec3(ray_world.x / ray_world.w, ray_world.y / ray_world.w, ray_world.z / ray_world.w) - currentCam->camera_pos);
		}

		// sets the cursor to the center of the screen
		//glfwSetCursorPos(window, Window::width / 2, Window::height / 2);

		// hides cursor away from screen
		// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}


}

static bool light_toggle = false;
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
		{
			shiftPressed = 1;
		}
		if (key == GLFW_KEY_W) {
			goForward = 1;
			goBackward = 0;
		}
		if (key == GLFW_KEY_S) {
			goForward = 0;
			goBackward = 1;
		}
		if (key == GLFW_KEY_A) {
			goLeft = 1;
			goRight = 0;
		}
		if (key == GLFW_KEY_D) {
			goRight = 1;
			goLeft = 0;
		}
		if (key == GLFW_KEY_E) {
			goUp = 1;
			goDown = 0;
		}
		if (key == GLFW_KEY_Q) {
			goDown = 1;
			goUp = 0;
		}
		if (key == GLFW_KEY_C) {
			showCoordinate = -showCoordinate + 1;
		}
		if (key == GLFW_KEY_I) {
			pickStyle--;
		}
		if (key == GLFW_KEY_O) {
			pickStyle++;
		}
		if (key == GLFW_KEY_0) {
			if (!light_toggle)
			{
				pickType = 0;
				pickStyle = 0;
			}
		}
		if (key == GLFW_KEY_1) {
			if (!light_toggle)
			{
				pickType = 1;
				pickStyle = 0;
			}
		}
		if (key == GLFW_KEY_L)
		{
			light_toggle = !light_toggle;
			std::cerr << "Enter light selection mode!" << std::endl;
		}
		if (light_toggle && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
		{
			int index = key - GLFW_KEY_0;
			lights.toggleLight(index);
			lights.updateShader(Shader_Model);
			std::cerr << "Light " << index << " is turned " << ((lights.getLightStatus(index)==1)?"on":"off") << "!" << std::endl;
		}
		if (light_toggle && key >= GLFW_KEY_F1 && key <= GLFW_KEY_F6)
		{
			int index = key - GLFW_KEY_F1 + 10;
			lights.toggleLight(index);
			lights.updateShader(Shader_Model);
			std::cerr << "Light " << index << " is turned " << ((lights.getLightStatus(index) == 1) ? "on" : "off") << "!" << std::endl;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
		{
			shiftPressed = 0;
		}
		if (key == GLFW_KEY_W) {
			goForward = 0;
		}
		if (key == GLFW_KEY_S) {
			goBackward = 0;
		}
		if (key == GLFW_KEY_A) {
			goLeft = 0;
		}
		if (key == GLFW_KEY_D) {
			goRight = 0;
		}
		if (key == GLFW_KEY_E) {
			goUp = 0;
		}
		if (key == GLFW_KEY_Q) {
			goDown = 0;
		}
	}
	if (action == GLFW_REPEAT) {
		
	}
}
