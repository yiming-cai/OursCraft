#include "OBJObject.h"

using namespace std;

// constructor: parse the file and initialize toWorld matrix
OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
	normalize_vertices();
	i_normal_mode = 0;

	// Create array object and buffers. 
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	// bind the normal vector
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(faces[0]), &faces[0], GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

OBJObject::~OBJObject()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
}

void OBJObject::normalize_vertices()
{
	// find the current boundaries of the model
	float xmax = std::numeric_limits<float>::min();
	float xmin = std::numeric_limits<float>::max();
	float ymax = std::numeric_limits<float>::min();
	float ymin = std::numeric_limits<float>::max();
	float zmax = std::numeric_limits<float>::min();
	float zmin = std::numeric_limits<float>::max();
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x > xmax)
		{
			xmax = vertices[i].x;
		}

		if (vertices[i].x < xmin)
		{
			xmin = vertices[i].x;
		}

		if (vertices[i].y > ymax)
		{
			ymax = vertices[i].y;
		}

		if (vertices[i].y < ymin)
		{
			ymin = vertices[i].y;
		}

		if (vertices[i].z > zmax)
		{
			zmax = vertices[i].z;
		}

		if (vertices[i].z < zmin)
		{
			zmin = vertices[i].z;
		}
	}

	// calculate the offset to move the position
	float x_offset = (xmin + xmax) / -2.0f;
	float y_offset = (ymin + ymax) / -2.0f;
	float z_offset = (zmin + zmax) / -2.0f;

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].x += x_offset;
		vertices[i].y += y_offset;
		vertices[i].z += z_offset;
	}
	
	xmax += x_offset;
	ymax += y_offset;
	zmax += z_offset;

	float scale = std::max(xmax, ymax);
	scale = std::max(scale, zmax);

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].x /= scale;
		vertices[i].y /= scale;
		vertices[i].z /= scale;
	}
}

void OBJObject::parse(const char *filepath) 
{
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data

	// initialize some local variables to temporarily store data
	std::ifstream file;
	float x, y, z;
	GLuint v1, v2, v3;
	
	// open the file and check if the file exists
	errno = 0;
	file.open(filepath);
	if (!file.is_open() || errno != 0)
	{
		char * str = nullptr;
		strerror_s(str, 80, errno);
		std::cerr << "Error loading file " << filepath << ": " << str << std::endl;
		exit(-1);
	}

	// define some variable for parsing
	int c1, c2;
	std::string temp;

	// main loop for reading the file
	while (std::getline(file, temp))
	{
		if (temp.size() > 1)
		{

			c1 = temp.at(0);
			c2 = temp.at(1);
		}
		else
		{
			continue;
		}

		// check for comment
		if (c1 == '#') 
		{
			// skip current line
			continue;
		}

		// check for vertex
		else if ((c1 == 'v') && (c2 == ' '))
		{
			// parse the vertex and insert it
			parseVertex(temp, x, y, z);
			vertices.push_back(glm::vec3(x,y,z));
		}

		// check for vertex normal
		else if ((c1 == 'v') && (c2 == 'n'))
		{
			// parse the normal, map it and insert it
			parseVertex(temp, x, y, z);
			glm::vec3 tempv = glm::normalize(glm::vec3(x, y, z));
			// disable mapping the vectors into [0, 1]
			//tempv.x = tempv.x * 0.5f + 0.5f;
			//tempv.y = tempv.y * 0.5f + 0.5f;
			//tempv.z = tempv.z * 0.5f + 0.5f;
			normals.push_back(tempv);
		}

		// checck for faces
		else if ((c1 == 'f') && (c2 == ' '))
		{
			parseFace(temp, v1, v2, v3);

			// 0-based index adjustment
			faces.push_back( glm::uvec3(v1-1,v2-1,v3-1) );
		}
	}

	//std::cout << filepath << ": " << vertices.size() << "vertices" << std::endl;
	//std::cout << filepath << ": " << normals.size() << "normals" << std::endl;
	//std::cout << filepath << ": " << faces.size() << "faces" << std::endl;
	file.close();
}

void OBJObject::parseFace(std::string str, 
	unsigned int & v1, unsigned int & v2, unsigned int & v3)
{
	std::string temp = "";
	int token_count = 0;
	std::string::size_type sz;     // alias of size_t

	for (int i = 0; i < str.size(); i++)
	{
		if ((str.at(i) == ' '))
		{
			if (token_count == 1)
			{
				v1 = std::stoi(temp, &sz);
				//temp = temp.substr(sz+2);
				//n1 = std::stoi(temp, &sz);
			}
			else if (token_count == 2)
			{
				v2 = std::stoi(temp, &sz);
				//temp = temp.substr(sz + 2);
				//n2 = std::stoi(temp, &sz);
			}
			else if (token_count > 2)
			{
				break;
			}
			token_count++;
			temp.clear();
		}
		else
		{
			temp.append(1, str.at(i));
		}
	}

	// the final element after the loop
	v3 = std::stoi(temp, &sz);
	//temp = temp.substr(sz + 2);
	//n3 = std::stoi(temp, &sz);
}

void OBJObject::parseVertex(std::string str, float & x, float & y, float & z)
{
	std::string temp = "";
	int token_count = 0;
	for (int i = 0; i < str.size(); i++)
	{
		if ( (str.at(i) == ' ') || (i == str.size() - 1) )
		{
			if (token_count == 1)
			{
				x = std::stof(temp, nullptr);
			}
			else if (token_count == 2)
			{
				y = std::stof(temp, nullptr);
			}
			else if (token_count > 2)
			{
				break;
			}
			token_count++;
			temp.clear();
		}
		else
		{
			temp.append(1, str.at(i));
		}
	}

	// final element 
	z = std::stof(temp, nullptr);
}

void OBJObject::increase_point_size()
{
	if (point_size <= 50.0f)
	{
		point_size += 0.5f;
		glPointSize(point_size);
	}
}

void OBJObject::decrease_point_size()
{
	if (point_size > 0.5f)
	{
		point_size -= 0.5f;
		glPointSize(point_size);
	}
}

void OBJObject::setPointSize()
{
	glPointSize(point_size);
}

void OBJObject::draw(GLuint shaderProgram)
{
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");
	normal_mode = glGetUniformLocation(shaderProgram, "normal_mode");

	GLint loc_k_s = glGetUniformLocation(shaderProgram, "k_s");
	GLint loc_k_d = glGetUniformLocation(shaderProgram, "k_d");
	GLint loc_k_a = glGetUniformLocation(shaderProgram, "k_a");
	GLint loc_def_c = glGetUniformLocation(shaderProgram, "shininess");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
	glUniform1i(normal_mode, i_normal_mode);
	glUniform3fv(loc_k_s, 1, &k_s[0]);
	glUniform3fv(loc_k_d, 1, &k_d[0]);
	glUniform3fv(loc_k_a, 1, &k_a[0]);
	glUniform1f(loc_def_c, shininess);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	// Tell OpenGL to draw with triangles, using all indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei) faces.size() * 3, GL_UNSIGNED_INT, 0);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void OBJObject::switchRenderMode()
{
	if (i_normal_mode == 0)
	{
		i_normal_mode = 1;
	}
	else
	{
		i_normal_mode = 0;
	}
}

void OBJObject::update()
{
	// spin all the time
	// this->spin(1.0f);
}

void OBJObject::spin(float deg)
{
	// This creates the matrix to rotate the cube
	this->toWorld = glm::rotate(this->toWorld, deg * degree , glm::vec3(0.0f, 1.0f, 0.0f));
}

// Reset the 3x3 sub-matrix
void OBJObject::reset(int arg)
{
	if (arg == RESET_ORIENTATION)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (i == j)
				{
					this->toWorld[i][j] = 1;
				}
				else
				{
					this->toWorld[i][j] = 0;
				}
			}
		}
	}

	// Reset the position
	else if (arg == RESET_POSITION)
	{
		// reset the last column, which stores the translation information
		this->toWorld[3][0] = 0;
		this->toWorld[3][1] = 0;
		this->toWorld[3][2] = 0;
	}
}

void OBJObject::scale(int opt)
{
	if (opt == SCALE_UP)
	{
		this->toWorld = glm::scale(this->toWorld, glm::vec3(1.25,1.25,1.25));
	}
	else if (opt == SCALE_DOWN)
	{
		this->toWorld = glm::scale(this->toWorld, glm::vec3(.8, .8, .8));
	}
}

void OBJObject::setSize(float s)
{
	toWorld[0][0] = s;
	toWorld[1][1] = s;
	toWorld[2][2] = s;
}

void OBJObject::orbit(int direction)
{
	if (direction == ORBIT_COUNTER_CW)
	{
		this->toWorld = glm::rotate(glm::mat4(1.0f), 5.0f * degree, glm::vec3(0.0f, 0.0f, 1.0f)) * this->toWorld;
	}
	else if (direction == ORBIT_CW)
	{
		this->toWorld = glm::rotate(glm::mat4(1.0f), 5.0f * degree, glm::vec3(0.0f, 0.0f, -1.0f)) * this->toWorld;
	}
}

void OBJObject::rotate(double p_xPos, double p_yPos, double xPos, double yPos)
{
	glm::vec3 v1 = trackBallMapping(p_xPos, p_yPos);
	glm::vec3 v2 = trackBallMapping(xPos, yPos);
	glm::vec3 a = glm::cross(v1, v2);
	float cos_theta = glm::dot(v1, v2) / (v1.length() * v2.length());
	float theta = acos(cos_theta);
	this->toWorld = glm::rotate(glm::mat4(1.0f), theta * degree , a) * this->toWorld;
}

glm::vec3 OBJObject::trackBallMapping(double xpos, double ypos)  
{
	glm::vec3 v;    // Vector v is the synthesized 3D position of the mouse location on the trackball
	float d;     // this is the depth of the mouse location: the delta between the plane through the center of the trackball and the z position of the mouse
	v.x = ( 2.0f* (float) xpos - (float) Window::width) / (float) Window::width;   // this calculates the mouse X position in trackball coordinates, which range from -1 to +1
	v.y = ( (float) Window::height - 2.0f* (float) ypos) / (float) Window::height;   // this does the equivalent to the above for the mouse Y position
	v.z = 0.0f;   // initially the mouse z position is set to zero, but this will change below
	d = glm::length(v);    // this is the distance from the trackball's origin to the mouse location, without considering depth (=in the plane of the trackball's origin)
	d = (d<1.0f) ? d : 1.0f;   // this limits d to values of 1.0 or less to avoid square roots of negative values in the following line
	v.z = sqrtf(1.001f - d*d);  // this calculates the Z coordinate of the mouse position on the trackball, based on Pythagoras: v.z*v.z + d*d = 1*1
	glm::normalize(v);
	return v;  // return the mouse location on the surface of the trackball
}

void OBJObject::setPosition(glm::vec3 pos)
{
	this->toWorld[3][0] = pos.x;
	this->toWorld[3][1] = pos.y;
	this->toWorld[3][2] = pos.z;
}

void OBJObject::translate(double x, double y)
{
	glm::vec3 temp( (float) x, (float) y, 0);
	float dim = (float) std::min(Window::width, Window::height);
	temp.x /= ( dim / 20.0f );
	temp.y /= -1.0f * ( dim / 20.0f );
	this->toWorld = glm::translate(glm::mat4(1.0f), temp) * this->toWorld;
}

void OBJObject::translate(int direction)
{
	glm::vec3 temp;

	// set the direction vector
	switch (direction)
	{
	case UP:
		temp = glm::vec3(0.0f, 1.0f, 0.0f);
		break;

	case DOWN:
		temp = glm::vec3(0.0f, -1.0f, 0.0f);
		break;

	case LEFT:
		temp = glm::vec3(-1.0f, 0.0f, 0.0f);
		break;

	case RIGHT:
		temp = glm::vec3(1.0f, 0.0f, 0.0f);
		break;

	case IN:
		temp = glm::vec3(0.0f, 0.0f, -1.0f);
		break;

	case OUT:
		temp = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	}

	// perform the translation
	this->toWorld = glm::translate(glm::mat4(1.0f), temp) * this->toWorld;
}

glm::vec3 OBJObject::getPosition()
{
	return glm::vec3(toWorld[3][0], toWorld[3][1], toWorld[3][2]);
}