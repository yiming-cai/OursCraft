#include "Window.h"

extern glm::mat4 P;
extern glm::mat4 V;
extern GLuint Shader_Geometry; 
extern GLuint Shader_Skybox;
extern GLuint Shader_Coordinate;
extern GLuint Shader_Model;
extern GLuint Shader_SimplePointer;
extern GLuint Shader_BoundBox;
extern GLuint Shader_DisplayLight;
extern GLuint Shader_Water;
extern GLuint Shader_Particle;
extern GLuint Shader_Tree;
std::vector<Cube *> cubeList;
std::vector<Camera *> cameraList;
std::vector<Model *> domino;
std::vector<Model *> otherModels;

int pickType = 0;
int pickStyle = 0;
int idCount = 0;
int leftMousePressed,rightMousePressed;
int leftMouseRepeated;
int disableFog;
int pickObjectFace;
int showCoordinate;
int goRight, goLeft, goUp, goDown, goForward, goBackward;
int displayLightOnCube;
int disableWater;
int disableToon;
float mouseX, mouseY;
glm::vec3 ray_dir;
Object* pickObject = NULL;

Cube *cube;
Skybox *skybox;
Camera *currentCam;
Coordinate *coordinate;
SimplePointer *centerRouter;
Fog *fog;
Water *water;
ParticleGenerator * fist;
std::vector<std::string> faces
{
	"../assets/night skybox/2/Right.png",
	"../assets/night skybox/2/Left.png",
	"../assets/night skybox/2/Up.png",
	"../assets/night skybox/2/Down.png",
	"../assets/night skybox/2/Back.png",
	"../assets/night skybox/2/Front.png"
};

//int domino_turn_point;
std::pair<int, int> domino_branch;
int first_branch_last;

const char* window_title = "GLFW Starter Project";

Model * hand;
Light lights;
LightDisplay * lightDisplay;

Sound * sound;
ALuint bgmRightSource, bgmLeftSource;
ALuint collisionBuffer;
ALuint collisionSource, collisionSource2;
Util util;
std::vector<Tree *> trees;

int keyPressed;
int shiftPressed;
// On some systems you need to change this to the absolute path

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 10.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

bool begin_domino = false;
bool show_boudingbox = false;
bool done_collision = false;
int count = 0;
int count_before = 0;
int domino_index = 0;


void Window::placeObject(int type, int *style)
{
	if (type == 0) return;
	if (pickObject != NULL) {
		glm::vec3 npos = pickObject->getObjectBiasPosition(pickObjectFace);
		switch (type) {
		case 1:
			if (*style >= CUBE_TEXTURE_NUM) *style = *style%CUBE_TEXTURE_NUM;
			if (*style < 0) *style = CUBE_TEXTURE_NUM + (*style%CUBE_TEXTURE_NUM);
			cube = new Cube(idCount++, 1, *style);
			cube->setPosition(npos.x, npos.y, npos.z);
			cubeList.push_back(cube);
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
	for (int i = 0; i < cubeList.size(); ++i) {
		//cubeList[i]->getId();
		float j = cubeList[i]->getCollisionValue(currentCam->camera_pos, ray_dir,&fff);
		if (j > 0 && j < value) {
			*face = fff;
			value = j;
			obj = cubeList[i];
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
	Shader_DisplayLight = LoadShaders(DISPLAYLIGHT_VERTEX_SHADER_PATH, DISPLAYLIGHT_FRAGMENT_SHADER_PATH);
	Shader_SimplePointer = LoadShaders(SIMPLE_POINTER_VERTEX_SHADER_PATH, SIMPLE_POINTER_FRAGMENT_SHADER_PATH);
	Shader_BoundBox = LoadShaders(BOUNDBOX_VERTEX_SHADER_PATH,BOUNDBOX_FRAGMENT_SHADER_PATH);
	Shader_Water = LoadShaders(WATER_VERTEX_SHADER_PATH, WATER_FRAGMENT_SHADER_PATH);
	Shader_Tree = LoadShaders(TREE_VERTEX_SHADER_PATH, TREE_FRAGMENT_SHADER_PATH);
	Shader_Particle = LoadShaders(PARTICLE_VERTEX_SHADER_PATH,PARTICLE_FRAGMENT_SHADER_PATH);
}

bool Window::loadFromFile(std::string filename)
{
	std::ifstream in(filename, std::ofstream::in);
	if (!in.is_open())
	{
		std::cerr << "Invalid file! Please make a save file first" << std::endl;
		return false;
	}

	for (int i = 0; i < cubeList.size(); i++)
	{
		delete cubeList[i];
	}
	cubeList.clear();
	std::cerr << (cubeList.size() == 0 ? "Cleared cube list!" : "Failed to clear cube list!") << std::endl;

	std::string line;
	bool readingCube = false;
	bool readingLight = false;
	Cube * cube_temp;
	while (std::getline(in, line))
	{
		std::vector<std::string> tokens = util.split(line, ',');
		if (tokens[0] == "#IDCOUNT")
		{
			int count = std::stoi(tokens[1], nullptr);
			idCount = count;
			std::cerr << "load id count: " << idCount << std::endl;
		}
		else if (tokens[0] == "#STARTCUBE")
		{
			std::cerr << "start to read cube lines..." << std::endl;
			readingCube = true;
		}
		else if (tokens[0] == "#ENDCUBE")
		{
			std::cerr << "stop to read cube lines..." << std::endl;
			readingCube = false;
		}
		else if (tokens[0] == "#STARTLIGHT")
		{
			readingLight = true;
		}
		else if (tokens[0] == "#ENDLIGHT")
		{
			readingLight = false;
		}
		else if (readingCube)
		{
			int id = std::stoi(tokens[FILE_INDEX_ID], nullptr);
			//if (id == -1) continue;
			float pos_x = std::stof(tokens[FILE_INDEX_POS_X], nullptr);
			float pos_y = std::stof(tokens[FILE_INDEX_POS_Y], nullptr);
			float pos_z = std::stof(tokens[FILE_INDEX_POS_Z], nullptr);
			float size = std::stof(tokens[FILE_INDEX_SIZE], nullptr);
			int texId = std::stoi(tokens[FILE_INDEX_TEXTUREID], nullptr);
			float color_x = std::stof(tokens[FILE_INDEX_COLOR_X], nullptr);
			float color_y = std::stof(tokens[FILE_INDEX_COLOR_Y], nullptr);
			float color_z = std::stof(tokens[FILE_INDEX_COLOR_Z], nullptr);
			if (texId != -1)
			{
				cube_temp = new Cube(id, size, texId);
				cube_temp->setPosition(pos_x, pos_y, pos_z);
				cubeList.push_back(cube_temp);
			}
			else
			{
				cube_temp = new Cube(id, size, glm::vec3(color_x, color_y, color_z));
				cube_temp->setPosition(pos_x, pos_y, pos_z);
				cubeList.push_back(cube_temp);
			}
		}
		else if (readingLight)
		{
			lights.readFromFile(in);
		}
		else
		{
			std::cerr << "Unknown file line: " << line << std::endl;
		}
	}

	std::cout << "Successfully loaded from " << filename << "!" << std::endl;
	return true;
}

void Window::initialize_objects()
{
	leftMousePressed = 0;
	leftMouseRepeated = 0;
	mouseX = mouseY = MOUSEPOS_INIT_VALUE;
	goRight = goLeft = goUp = goDown = goForward = goBackward = 0;
	showCoordinate = 0;
	disableFog = 1;
	disableWater = 1;
	loadAllShader();

	//	printf("LoadShaders Finished!2 %d\n", Shader_Geometry);
	std::stringstream s;
	s << OUTPUTFILELOCATION << "1";
	if (!loadFromFile(s.str()))
	{
		int min = -2, max = 2;
		for (int i = min; i <= max; ++i)
		{
			for (int j = min; j <= max; ++j)
			{
				//cube = new Cube(idCount++, 1, glm::vec3(0.94, 1, 1));
				cube = new Cube(idCount++, 1, 0);
				cube->setPosition(i, GROUND_LEVEL - 1, j);
				cubeList.push_back(cube);
			}
		}
	}


	bindedCubeVAO = false;

	// toon shader
	GLuint temp;
	disableToon = 0;
	glUseProgram(Shader_Model);
	temp = glGetUniformLocation(Shader_Model, "disableToon");
	glUniform1i(temp, disableToon);
	glUseProgram(Shader_Geometry);
	temp = glGetUniformLocation(Shader_Geometry, "disableToon");
	glUniform1i(temp, disableToon);
	glUseProgram(Shader_DisplayLight);
	temp = glGetUniformLocation(Shader_DisplayLight, "disableToon");
	glUniform1i(temp, disableToon);
	glUseProgram(Shader_Tree);
	temp = glGetUniformLocation(Shader_Tree, "disableToon");
	glUniform1i(temp, disableToon);

	// toggle them to on
	displayLightOnCube = 0;
	glUseProgram(Shader_Geometry);
	temp = glGetUniformLocation(Shader_Geometry, "disableLight");
	glUniform1i(temp, displayLightOnCube);
	glUseProgram(Shader_Water);
	temp = glGetUniformLocation(Shader_Water, "disableLight");
	glUniform1i(temp, displayLightOnCube);
	// init skybox
	skybox = new Skybox(idCount++, 1000, &faces);

	// init cam;
	currentCam = new Camera(idCount++, glm::vec3(0, 1, 0), glm::vec3(0, 1, -1), glm::vec3(0, 1, 0));

	// init cor;
	coordinate = new Coordinate(idCount++, 100);

	// init pointer
	centerRouter = new SimplePointer(idCount++, 0, 0, glm::vec3(1, 0, 0));
	ray_dir = glm::vec3(0, 0.5, -1);

	// init fog
	fog = new Fog(glm::vec3(0.2, 0.2, 0.2), currentCam, 2, 15, 0.1);
	fog->addShader(Shader_Geometry);
	fog->addShader(Shader_DisplayLight);
	fog->addShader(Shader_Model);
	fog->addShader(Shader_Water);
	fog->addShader(Shader_Tree);
	fog->addShader(Shader_Skybox);
	fog->fogUpdate(disableFog);

	printf("Init All Done\n PLEASE TYPE 1-4 to select Object, and use I O to select Texture\n");


	// init water;
	
	water = new Water(idCount++, 300, 200, 0.2, 0, 1, 0,skybox ->getTexture(), currentCam);
	water->setPosition(-150, GROUND_LEVEL - 2, 150);
	// Enables backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// ------------ for the BGM ----------------------------------------------
	sound = new Sound(currentCam);

	// generate a sound buffer to store sound files
	ALuint buf = sound->generateBuffer("../assets/sounds/song_mono.wav");
	collisionBuffer = sound->generateBuffer("../assets/sounds/collision.wav");
	glm::vec3 rightSpeakerPos(5.0f, 1.0f, -28.0f);
	glm::vec3 leftSpeakerPos(-2.0f, 1.0f, -28.0f);

	// generate a source that plays the buffer
	bgmRightSource = sound->generateSource(rightSpeakerPos);
	bgmLeftSource = sound->generateSource(leftSpeakerPos);
	collisionSource = sound->generateSource(glm::vec3(0));
	collisionSource2 = sound->generateSource(glm::vec3(0));

	// bind the buffer to the source
	sound->bindSourceToBuffer(bgmRightSource, buf);
	sound->bindSourceToBuffer(bgmLeftSource, buf);
	sound->bindSourceToBuffer(collisionSource, collisionBuffer);
	sound->bindSourceToBuffer(collisionSource2, collisionBuffer);

	// make the bgm loop (don't use this if you just want it to play once)
	sound->setSourceLooping(bgmRightSource, true);	// Only if you want the sound to keep on looping!
	sound->setSourceLooping(bgmLeftSource, true);
	sound->setSourceLooping(collisionSource, false);
	sound->setSourceLooping(collisionSource2, false);

	// play the sound now
	sound->playSourceSound(bgmRightSource);
	sound->playSourceSound(bgmLeftSource);

	// speaker models
	Model * speaker = new Model("../assets/speaker/speaker.obj");
	speaker->centerAndScale(2.0f);
	speaker->setModelMatrix(glm::translate(glm::mat4(1.0f), rightSpeakerPos));
	speaker->setCamera(currentCam);
	speaker->initShader(Shader_Model);
	otherModels.push_back(speaker);

	speaker = new Model("../assets/speaker/speaker.obj");
	speaker->centerAndScale(2.0f);
	speaker->setModelMatrix(glm::translate(glm::mat4(1.0f), leftSpeakerPos));
	speaker->setCamera(currentCam);
	speaker->initShader(Shader_Model);
	otherModels.push_back(speaker);
	// -----------------------------------------------------------------------------


	// ------------------------ DOMINO --------------------------------------
	// init domino
	int domino_scale = 1.5f;

	// Define a list of points
	Model * model1;
	const static glm::vec3 P1 = { -34.5, 0, -1.5 };
	const static glm::vec3 P2 = { 1.5, 0, -1.5 };
	const static glm::vec3 P3 = { 1.5, 0, -65.5 };
	const static glm::vec3 P4 = { 25.5, 0, -65.5 };
	const static glm::vec3 P5 = { 25.5, 0, -18.5 };
	const static glm::vec3 P6 = { -28.5, 0, -64.5 };
	const static glm::vec3 P7 = { -28.5, 0, -11.5 };
	const static glm::vec3 P8 = { -11.5, 0, -11.5 };
	const static glm::vec3 P9 = { -11.5, 0, -1.5 };
	const static float straight_spacing = 0.5f*1.34f*domino_scale;
	const static float diagonal_spacing = 0.5f*1.50f*domino_scale;

	// loop 1, from p2 to p3;
	glm::vec3 start = P2, end = P3;
	//start.x += 0.5f; start.z += 0.5f; end.x += 0.5f; end.y += 0.5;
	int max_number = int( glm::length(end - start) / straight_spacing );
	glm::vec3 direction = glm::normalize(end - start);
	float rotation = 0;

	for (int i = 0; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);


		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		int asdf = 0;
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix());
		
		model1->setBoundingBox();
		model1->bounding_box->update();

		domino.push_back(model1);
	}

	domino_branch.first = domino.size() - 1;

	rotation = 45;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());
		
		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	// loop 2, from p3 to p6;
	start = P3, end = P6;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 90.0f;

	for (int i = 1; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix( glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)) );
		model1->setModelMatrix( glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix() );

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	rotation = 135;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	// loop 2, from p3 to p6;
	start = P6, end = P7;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 180.0f;

	for (int i = 1; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	rotation = 225;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	// loop 2, from p3 to p6;
	start = P7, end = P8;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 270.0f;

	for (int i = 1; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	rotation = 225;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	// loop 2, from p3 to p6;
	start = P8, end = P9;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 180.0f;

	for (int i = 1; i < max_number+1; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	rotation = 135;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	// loop 2, from p3 to p6;
	start = P9, end = P1;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 90.0f;

	for (int i = 1; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}
	first_branch_last = domino.size() - 1;
	// --------------- branch two ----------------------------

	end = P3;
	rotation = 315;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());
		
		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}
	domino_branch.second = domino.size() - 1;

	// loop 2, from p3 to p6;
	start = P3, end = P4;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 270.0f;

	for (int i = 1; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix( glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)) );
		model1->setModelMatrix( glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix() );

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	rotation = 225;
	{
		//domino_turn_point = domino.size();

		model1 = new Model("../cuboid.obj");
		model1->centerAndScale(domino_scale);

		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), end + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}

	// loop 2, from p3 to p6;
	start = P4, end = P5;
	//start.x += 1.5f; start.z += 0.5f; end.x += 1.5f; end.y += 0.5;
	max_number = int(glm::length(end - start) / straight_spacing);
	direction = glm::normalize(end - start);
	rotation = 180.0f;

	for (int i = 1; i < max_number; i++)
	{
		model1 = new Model("../cuboid.obj");

		model1->centerAndScale(domino_scale);
		model1->setCamera(currentCam);
		model1->initShader(Shader_Model);

		std::vector<float> minmax = model1->getMinMaxValues();
		glm::vec3 domino_displacement = -1.0f*model1->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], 0, 1.0f);
		model1->setModelMatrix(glm::rotate(glm::mat4(1.0f), rotation * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)));
		model1->setModelMatrix(glm::translate(glm::mat4(1.0f), start + i * straight_spacing * direction + domino_displacement)*model1->getUModelMatrix());

		model1->setBoundingBox();
		model1->bounding_box->update();
		domino.push_back(model1);
	}



	hand = new Model("../hand.obj");
	hand->centerAndScale(1.0f);
	hand->setModelMatrix(glm::translate(glm::mat4(1.0f), { 1.39f,0.99f,0.01f })*hand->getUModelMatrix());
	//hand->setModelMatrix(glm::rotate(glm::mat4(1.0f), 45.0f*glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)) * hand->getUModelMatrix());
	hand->setBoundingBox();
	hand->bounding_box->update();
	hand->setCamera(currentCam);
	hand->initShader(Shader_Model);
	
	//init particle
	fist = new ParticleGenerator();



	// Enable depth buffering
	// glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// create a test light
	lights = Light();
	lights.randInit();
	lights.initializeShader(Shader_Model);
	lights.initializeShader(Shader_Geometry);
	lights.initializeShader(Shader_Water);
	lights.initializeShader(Shader_Tree);
	lights.turnAllLightOn();
	lights.updateAllShader();


	lightDisplay = new LightDisplay(&lights, currentCam);
	lightDisplay->initShader(Shader_DisplayLight);
	lightDisplay->update(Shader_DisplayLight);
	// --------------------------------------------------------------

	// ----------------- Other models and test models -----------------------------
	std::vector<glm::vec3> cockle_pos = { {16,0,-8}, { 18,0,-11 },{ 20,0,-14 },{ 27,0,-21 },{ 26,0,-32 },{ 27,0,-24 },{ 30,0,-42 },{ 30,0,-48 },{ 29,0,-63 },
										  { 26,0,-65 },{ 14,0,-70 },{ 0,0,-68 },{ 5,0,-69 },{ -30,0,-60 },{ -30,0,-50 },{ -28,0,-39 },{ -28,0,-26 },{ -25,0,-21 },
										  { -27,0,-13 },{ -19,0,-11 },{ -25,0,-42 },{ 9,0,-68 },{ -25,0,-66 },{ -21,0,-52 },{ -24,0,-56 },{ -29,0,-64 },{ -27,0,-52 },
										  { -26,0,-27 },{ -23,0,-13 },{ 15,0,-62 },{ 24,0,-54 },{ 27,0,-37 },{ 9,0,-5 },{ 23,0,-17 },{ -11,0,-12 } };
	srand(32132);
	for (int i = 0; i < cockle_pos.size(); i++)
	{
		Model * model = new Model("../cockle/common-cockle.obj");
		model->centerAndScale( ( float(rand()%100)/200.0f) + 0.2f );
		model->setModelMatrix(glm::translate(glm::mat4(1.0f), -1.0f * glm::vec3(model->getModelMatrix() * glm::vec4(0, model->getMinMaxValues()[Model::INDEX_Y_MIN], 0, 1))));
		model->setModelMatrix(glm::translate(model->getUModelMatrix(), cockle_pos[i]));
		model->setModelMatrix(glm::rotate(model->getUModelMatrix(), 180.0f * glm::pi<float>() / 180.0f, glm::vec3(1,0,0)));
		model->setModelMatrix(glm::rotate(model->getUModelMatrix(), float(rand()%180) * glm::pi<float>()/180.0f, glm::vec3( float(rand()%100)/100.0f, float(rand() % 100) / 100.0f, float(rand() % 100) / 100.0f)));
		model->setCamera(currentCam);
		model->initShader(Shader_Model);
		otherModels.push_back(model);
	}
	// ------------------------------------------------------------------------------
	
	// ------------------- for rendering trees -----------------------------------
	std::vector< std::pair<float, float>> tree_locations = { {8,-57},{12,-57},{16,-57},{ 19,-57 },{ 21,-55 },{ 21,-52 },{ 21,-49 },{ 21,-46 },{ 21,-43 },{ 21,-40 },
															 { 21,-37 },{ 21,-34 },{ 21,-31 },{ 21,-28 },{ 21,-25 },{ 21,-22 },{ 18,-21 },{ 14,-21 },{ 10,-21 },
															 { 6,-21 },{ -6,-21 },{ -10,-21 },{ -13,-21 },{ -16,-21 },{ -19,-22 },{ -19,-25 },{ -19,-28 },{ -19,-32 },
															 { -19,-36 },{ -19,-40 },{ -19,-44 },{ -19,-48 },{ -19,-52 },{ -19,-56 },{ -17,-57 } };
	srand(16);
	for (int i = 0; i < tree_locations.size(); i++)
	{
		Tree * newTree = new Tree(rand());
		trees.push_back(newTree);
		newTree->setPosition(tree_locations[i].first, tree_locations[i].second);
	}
	// ---------------------------------------------------------------------------
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	glDeleteProgram(Shader_Geometry);
	glDeleteProgram(Shader_Skybox);
	glDeleteProgram(Shader_Coordinate);
	glDeleteProgram(Shader_Model);
	glDeleteProgram(Shader_DisplayLight);
	glDeleteProgram(Shader_SimplePointer);
	glDeleteProgram(Shader_BoundBox);
	glDeleteProgram(Shader_Water);
	glDeleteProgram(Shader_Tree);
	glDeleteProgram(Shader_Particle);
	delete(hand);
	delete(fist);
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

	//fake the domino collision
	
	if (begin_domino == true) {
		
		if (!hand->bounding_box_hand->check_collision(domino[0]->bounding_box)&& !done_collision) {
			hand->setModelMatrix(hand->getUModelMatrix()*glm::translate(glm::mat4(1.0f), {0,0,-0.1f}) );
			if (hand->bounding_box_hand->collision) {
				done_collision = true;
			}

			bool collidedNow = domino[0]->bounding_box->collision;
			if (collidedNow)
			{

				sound->updateSourcePosition(collisionSource, domino[0]->getBoundingSphere().first);
				sound->playSourceSound(collisionSource);

			}
		}
			
		fist->update(0.1f, 1000, { 0.1f,0.1f,0.1f });
		
		glm::mat4 m1, m2, m3;
		
		for (int i = 0; i < domino.size()-1; ++i) {
			if (domino[i]->bounding_box->collision && domino[i]->bounding_box->count_rotate <=75) {

				float angle = 1.0f - float(domino[i]->bounding_box->count_rotate) / 100.0f;
				angle *= 10.0f;

				std::vector<float> minmax = domino[i]->getMinMaxValues();
				glm::vec3 ref_p1 = domino[i]->getModelMatrix() * glm::vec4( 0, 0, minmax[Model::INDEX_Z_MAX],1.0f );
				glm::vec3 ref_p2 = domino[i]->getModelMatrix() * glm::vec4(0, 0, minmax[Model::INDEX_Z_MIN], 1.0f);
				glm::vec3 dir = glm::normalize(ref_p2 - ref_p1); // direction domino is facing
				glm::vec3 up = glm::vec3(0, 1, 0);
				glm::vec3 axis = glm::normalize(glm::cross(up,dir));
				glm::vec3 domino_displacement = domino[i]->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN] , minmax[Model::INDEX_Z_MIN], 1.0f);
				glm::mat4 returnToOrigin = glm::translate(glm::mat4(1.0f), -1.0f*domino_displacement);
				glm::mat4 returnToPosition = glm::translate(glm::mat4(1.0f), domino_displacement);
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle *glm::pi<float>() / 180.0f, axis);

				domino[i]->setModelMatrix( returnToPosition * rotation * returnToOrigin * domino[i]->getUModelMatrix() );

				domino[i]->bounding_box->count_rotate += angle;
				domino[i]->bounding_box->update();

			}

			bool notCollidedBefore = !(domino[i + 1]->bounding_box->collision);
			domino[i]->bounding_box->check_collision(domino[i + 1]->bounding_box);
			bool collidedNow = domino[i + 1]->bounding_box->collision;
			if (notCollidedBefore && collidedNow)
			{
				if (i <= first_branch_last)
				{
					sound->updateSourcePosition(collisionSource, domino[i + 1]->getBoundingSphere().first);
					sound->playSourceSound(collisionSource);
				}
				else
				{
					sound->updateSourcePosition(collisionSource2, domino[i + 1]->getBoundingSphere().first);
					sound->playSourceSound(collisionSource2);
				}
			}

			if (i == domino_branch.first)
			{
				domino[i]->bounding_box->check_collision(domino[domino_branch.second]->bounding_box);
			}
			
		}

		if (domino[domino.size() - 1]->bounding_box->collision == true && domino[domino.size()-1]->bounding_box->count_rotate <= 89) {
			int i = domino.size() - 1;

			float angle = 1.0f - float(domino[i]->bounding_box->count_rotate) / 100.0f;
			angle *= 10.0f;

			std::vector<float> minmax = domino[i]->getMinMaxValues();
			glm::vec3 ref_p1 = domino[i]->getModelMatrix() * glm::vec4(0, 0, minmax[Model::INDEX_Z_MAX], 1.0f);
			glm::vec3 ref_p2 = domino[i]->getModelMatrix() * glm::vec4(0, 0, minmax[Model::INDEX_Z_MIN], 1.0f);
			glm::vec3 dir = glm::normalize(ref_p2 - ref_p1); // direction domino is facing
			glm::vec3 up = glm::vec3(0, 1, 0);
			glm::vec3 axis = glm::normalize(glm::cross(up, dir));
			glm::vec3 domino_displacement = domino[i]->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], minmax[Model::INDEX_Z_MIN], 1.0f);
			glm::mat4 returnToOrigin = glm::translate(glm::mat4(1.0f), -1.0f*domino_displacement);
			glm::mat4 returnToPosition = glm::translate(glm::mat4(1.0f), domino_displacement);
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle *glm::pi<float>() / 180.0f, axis);

			domino[i]->setModelMatrix(returnToPosition * rotation * returnToOrigin * domino[i]->getUModelMatrix());

			domino[i]->bounding_box->count_rotate += angle;
			domino[i]->bounding_box->update();
		}

		if (domino[first_branch_last]->bounding_box->collision == true && domino[first_branch_last]->bounding_box->count_rotate <= 89) {
			int i = first_branch_last;

			float angle = 1.0f - float(domino[i]->bounding_box->count_rotate) / 100.0f;
			angle *= 10.0f;

			std::vector<float> minmax = domino[i]->getMinMaxValues();
			glm::vec3 ref_p1 = domino[i]->getModelMatrix() * glm::vec4(0, 0, minmax[Model::INDEX_Z_MAX], 1.0f);
			glm::vec3 ref_p2 = domino[i]->getModelMatrix() * glm::vec4(0, 0, minmax[Model::INDEX_Z_MIN], 1.0f);
			glm::vec3 dir = glm::normalize(ref_p2 - ref_p1); // direction domino is facing
			glm::vec3 up = glm::vec3(0, 1, 0);
			glm::vec3 axis = glm::normalize(glm::cross(up, dir));
			glm::vec3 domino_displacement = domino[i]->getModelMatrix() * glm::vec4(0, minmax[Model::INDEX_Y_MIN], minmax[Model::INDEX_Z_MIN], 1.0f);
			glm::mat4 returnToOrigin = glm::translate(glm::mat4(1.0f), -1.0f*domino_displacement);
			glm::mat4 returnToPosition = glm::translate(glm::mat4(1.0f), domino_displacement);
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle *glm::pi<float>() / 180.0f, axis);

			domino[i]->setModelMatrix(returnToPosition * rotation * returnToOrigin * domino[i]->getUModelMatrix());

			domino[i]->bounding_box->count_rotate += angle;
			domino[i]->bounding_box->update();
		}

	}


	//water 
	if(!disableWater) water->waterUpdate();

	// fog
	fog->fogUpdate(disableFog);

	sound->updateListener();
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


	// draw cube!!!!!
	GLuint temp = glGetUniformLocation(Shader_Geometry, "cam_look_at");
	drawingCube = true;
	for (int i = 0; i < cubeList.size();++i)
		cubeList[i]->draw(glm::mat4(1.0f));
	bindedCubeVAO = false;
	drawingCube = false;
	
	//draw particle
	glUseProgram(Shader_Particle);
	fist->draw(Shader_Particle);
	


	// draw water;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(!disableWater) water->draw(glm::mat4(1.0f));
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// test draw model

	
	// --------- These are all boundbox shader -----------------------

	if (show_boudingbox == true) {
		glUseProgram(Shader_BoundBox);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int i = 0; i < domino.size(); i++)
		{
			domino[i]->bounding_box->draw(Shader_BoundBox);
		}
		hand->bounding_box_hand->draw(Shader_BoundBox);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//---------------------------------------------------- 

	// ------------- These are all Shader_Model, group them together for optimization -----------
	glUseProgram(Shader_Model);

	// render hand
	hand->render(Shader_Model);

	// render models
	for (int i = 0; i < otherModels.size(); i++)
	{
		otherModels[i]->render(Shader_Model);
	}

	// render dominos
	for (int i = 0; i < domino.size(); i++)
	{
		domino[i]->render(Shader_Model);
	}
	// -------------------------------------------------------------------------------------------

	lightDisplay->render(Shader_DisplayLight);

	// ----------- These are all Shader_Tree -----------------------------------------------------
	glUseProgram(Shader_Tree);
	for (int i = 0; i < trees.size(); i++)
	{
		trees[i]->render(Shader_Tree);
	}
	// ------------------------------------------------------------------------------------------
	glfwPollEvents();

	// Swap buffers
	glfwSwapBuffers(window);
	
}

void Window::mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
	{
		rightMousePressed = 1;
		if (pickObject != NULL) {
			for (int i = 0; i < cubeList.size(); ++i) {
				if (cubeList[i]->getId() == pickObject->getId()) {
					Cube * ptr = cubeList[i];
					cubeList.erase(cubeList.begin() + i);
					delete ptr;
					break;
				}
			}
		}
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && (action == GLFW_PRESS)) {
		leftMousePressed = 1;
		placeObject(pickType, &pickStyle);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && (action == GLFW_REPEAT))
	{
		leftMouseRepeated = 1;
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
		leftMouseRepeated = 0;
		mouseX = mouseY = MOUSEPOS_INIT_VALUE;
	}
}

void Window::mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

void Window::mousePos_callback(GLFWwindow* window, double xpos, double ypos) {

	//if (leftMousePressed == 0) {
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
	//}


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
		if (key == GLFW_KEY_R)
		{
			lights.setSeed(rand());
			lights.randInit();
			lights.turnAllLightOn();
			lights.updateAllShader();
			lightDisplay->update(Shader_DisplayLight);
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
		if (key == GLFW_KEY_T) {
			displayLightOnCube = (displayLightOnCube == 1 ? 0 : 1);
			
			// toggle geometry shader
			glUseProgram(Shader_Geometry);
			GLuint temp = glGetUniformLocation(Shader_Geometry, "disableLight");
			glUniform1i(temp, displayLightOnCube);

			// toggle water shader
			glUseProgram(Shader_Water);
			temp = glGetUniformLocation(Shader_Water, "disableLight");
			glUniform1i(temp, displayLightOnCube);
		}
		if (key == GLFW_KEY_Y)
		{
			disableToon = (disableToon == 1 ? 0 : 1);
			GLuint temp;
			glUseProgram(Shader_Model);
			temp = glGetUniformLocation(Shader_Model, "disableToon");
			glUniform1i(temp, disableToon);
			glUseProgram(Shader_Geometry);
			temp = glGetUniformLocation(Shader_Geometry, "disableToon");
			glUniform1i(temp, disableToon);
			glUseProgram(Shader_DisplayLight);
			temp = glGetUniformLocation(Shader_DisplayLight, "disableToon");
			glUniform1i(temp, disableToon);
			glUseProgram(Shader_Tree);
			temp = glGetUniformLocation(Shader_Tree, "disableToon");
			glUniform1i(temp, disableToon);
		}
		if (key == GLFW_KEY_F) {
			disableFog = -disableFog + 1;
		}
		if (key == GLFW_KEY_I) {
			pickStyle--;
		}
		if (key == GLFW_KEY_O) {
			pickStyle++;
		}
		if (key == GLFW_KEY_J) {
			disableWater = -disableWater + 1;
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
			std::cerr << (light_toggle ? "Enter light selection mode!" : "Exiting light selection mode!") << std::endl;
		}
		if (key == GLFW_KEY_Z)
		{
			begin_domino = !begin_domino;
		}
		if (key == GLFW_KEY_X)
		{
			show_boudingbox = !show_boudingbox;
		}
		// p for pausing or resuming
		if (key == GLFW_KEY_P)
		{
			if (sound->isSourcePlaying(bgmLeftSource) || sound->isSourcePlaying(bgmRightSource))
			{
				sound->pauseSourceSound(bgmLeftSource);
				sound->pauseSourceSound(bgmRightSource);
			}
			else
			{
				sound->playSourceSound(bgmLeftSource);
				sound->playSourceSound(bgmRightSource);
			}
		}
		if (light_toggle && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
		{
			int index = key - GLFW_KEY_0;
			lights.toggleLight(index);
			lights.updateAllShader();
			lightDisplay->update(Shader_DisplayLight);
			std::cerr << "Light " << index << " is turned " << ((lights.getLightStatus(index)==1)?"on":"off") << "!" << std::endl;
		}
		if (light_toggle && key >= GLFW_KEY_F1 && key <= GLFW_KEY_F6)
		{
			int index = key - GLFW_KEY_F1 + 10;
			lights.toggleLight(index);
			lights.updateAllShader();
			lightDisplay->update(Shader_DisplayLight);
			std::cerr << "Light " << index << " is turned " << ((lights.getLightStatus(index) == 1) ? "on" : "off") << "!" << std::endl;
		}

		// use ctrl + f1 to f12 to save to 12 different file locations
		// disabled when light mode is on
		if ( (mods & GLFW_MOD_CONTROL) != 0)
		{
			if (!light_toggle && key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12)
			{
				int file = key - GLFW_KEY_F1 + 1;
				std::stringstream str;
				str << "#IDCOUNT" << "," << idCount << std::endl;
				str << "#STARTCUBE" << std::endl;
				for (int i = 0; i < cubeList.size(); i++)
				{
					CubeInfo info = cubeList[i]->getCubeInfo();
					str << info.id << ","
						<< info.position.x << ","
						<< info.position.y << ","
						<< info.position.z << ","
						<< info.size << ","
						<< info.textureId << ","
						<< info.color.x << ","
						<< info.color.y << ","
						<< info.color.z << std::endl;
				}
				str << "#ENDCUBE" << std::endl;

				std::stringstream filename ;
				filename << OUTPUTFILELOCATION << file;
				std::ofstream out(filename.str(), std::ofstream::out);
				out << str.str();

				// For recording the light status
				str << "#STARTLIGHT" << std::endl;
				lights.writeToFile(out);
				str << "#ENDLIGHT" << std::endl;

				out.close();
				std::cout << "Successfully saved to " << filename.str() << "!" << std::endl;
			}
		}

		// use shift + f1 to f12 to load from 12 different locations
		// disabled in light mode
		if ((mods & GLFW_MOD_SHIFT) != 0)
		{
			if (!light_toggle && key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12)
			{
				int file = key - GLFW_KEY_F1 + 1;
				std::stringstream filename;
				filename << OUTPUTFILELOCATION << file;
				loadFromFile(filename.str());
				
			}
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
