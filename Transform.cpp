#include "Transform.h"

void Transform::addChild(Node * child)
{
	children.push_back(child);
}

void Transform::removeChild(Node * child)
{
	for (int i = 0; i < children.size(); i++)
	{
		// simple pointer compare to find the child
		if (children[i] == child)
		{
			children.erase(children.begin() + i);
			break;
		}
	}
}

void Transform::draw(GLuint shaderProgram, glm::mat4 C)
{
	glm::mat4 newMat = C * M;
	for (auto child : children)
	{
		child->draw(shaderProgram, newMat);
	}
}

void Transform::update()
{
	for (auto child : children)
	{
		child->update();
	}
}

Transform::Transform()
{
}

Transform::~Transform()
{

}

void Robot::update()
{
	// move the hand back and forth
	if (pos_angle)
	{
		hand_angle += 3.0f;
		t_left_arm->M = glm::rotate(glm::mat4(1.0f), 3.0f * glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * t_left_arm->M;
		t_right_arm->M = glm::rotate(glm::mat4(1.0f), -3.0f * glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * t_right_arm->M;
		if (hand_angle > 50.0f) pos_angle = false;
	}
	else
	{
		hand_angle -= 3.0f;
		t_left_arm->M = glm::rotate(glm::mat4(1.0f), -3.0f * glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * t_left_arm->M;
		t_right_arm->M = glm::rotate(glm::mat4(1.0f), 3.0f * glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * t_right_arm->M;
		if (hand_angle < -50.0f) pos_angle = true;
	}

	// move the leg back and forth
	if (pos_leg)
	{
		leg_pos += 0.03f;
		t_left_leg->M = glm::translate(t_left_leg->M, glm::vec3(0.0f, 0.0f, 0.03f));
		t_right_leg->M = glm::translate(t_right_leg->M, glm::vec3(0.0f, 0.0f, -0.03f));
		if (leg_pos > 0.5f) pos_leg = false;
	}
	else
	{
		leg_pos -= 0.03f;
		t_left_leg->M = glm::translate(t_left_leg->M, glm::vec3(0.0f, 0.0f, -0.03f));
		t_right_leg->M = glm::translate(t_right_leg->M, glm::vec3(0.0f, 0.0f, 0.03f));
		if (leg_pos < -0.5f) pos_leg = true;
	}

	// rotate eyeballs
	//t_left_eyeball->M = glm::rotate(glm::mat4(1.0f), 3.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * t_left_eyeball->M;
	//t_right_eyeball->M = glm::rotate(glm::mat4(1.0f), 3.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * t_right_eyeball->M;
}

Robot::Robot()
{
	t_body = new Transform();
	t_body->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>()/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	t_body->addChild(new Body());
	addChild(t_body);

	t_left_leg = new Transform();
	t_left_leg->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	t_left_leg->M = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * t_left_leg->M;
	t_left_leg->M = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, -1.0f, 0.0f)) * t_left_leg->M;
	t_left_leg->addChild(new Leg());
	addChild(t_left_leg);

	t_right_leg = new Transform();
	t_right_leg->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	t_right_leg->M = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * t_right_leg->M;
	t_right_leg->M = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, -1.0f, 0.0f)) * t_right_leg->M;
	t_right_leg->addChild(new Leg());
	addChild(t_right_leg);

	t_left_arm = new Transform();
	t_left_arm->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	t_left_arm->M = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * t_left_arm->M;
	t_left_arm->M = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, -0.1f, 0.0f)) * t_left_arm->M;
	t_left_arm->addChild(new Arm());
	addChild(t_left_arm);

	t_right_arm = new Transform();
	t_right_arm->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	t_right_arm->M = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * t_right_arm->M;
	t_right_arm->M = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.1f, 0.0f)) * t_right_arm->M;
	t_right_arm->addChild(new Arm());
	addChild(t_right_arm);

	t_left_eyeball = new Transform();
	t_left_eyeball->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	t_left_eyeball->M = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 2.0f, 0.8f)) * t_left_eyeball->M;
	t_left_eyeball->addChild(new Eyeball());
	addChild(t_left_eyeball);

	t_right_eyeball = new Transform();
	t_right_eyeball->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	t_right_eyeball->M = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.0f, 0.8f)) * t_right_eyeball->M;
	t_right_eyeball->addChild(new Eyeball());
	addChild(t_right_eyeball);

	//t_left_antenna = new Transform();
	//t_left_antenna->M = glm::rotate(glm::mat4(1.0f), 120.0f*glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//t_left_antenna->M = glm::rotate(glm::mat4(1.0f), 15.0f*glm::pi<float>() / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * t_left_antenna->M;
	//t_left_antenna->M = glm::rotate(glm::mat4(1.0f), 50.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * t_left_antenna->M;
	//t_left_antenna->M = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 3.1f, 0.1f)) * t_left_antenna->M;
	//t_left_antenna->addChild(new Antenna());
	//addChild(t_left_antenna);

	//t_right_antenna = new Transform();
	//t_right_antenna->M = glm::rotate(glm::mat4(1.0f), -120.0f*glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//t_right_antenna->M = glm::rotate(glm::mat4(1.0f), -15.0f*glm::pi<float>() / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * t_right_antenna->M;
	//t_right_antenna->M = glm::rotate(glm::mat4(1.0f), -50.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * t_right_antenna->M;
	//t_right_antenna->M = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 3.1f, 0.1f)) * t_right_antenna->M;
	//t_right_antenna->addChild(new Antenna());
	//addChild(t_right_antenna);

	// note that im using a body model for the head
	t_head = new Transform();
	t_head->M = glm::rotate(glm::mat4(1.0f), 90.0f*glm::pi<float>() / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	t_head->M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)) * t_head->M;
	t_head->addChild(new Head());
	addChild(t_head);
}

Robot::~Robot()
{
	for (auto child : children)
	{
		delete child;
		child = nullptr;
	}
}

void ArmyOfRobot::update()
{
	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.02f));

	if (children[0] != nullptr)
	{
		children[0]->update();
	}
}

ArmyOfRobot::ArmyOfRobot()
{
	robot = new Robot();
	robot->M = glm::mat4(1.0f);
	for (int i = 0-grid_size/2; i < grid_size/2; i++)
	{
		for (int j = -grid_size/2; j < grid_size/2 ; j++)
		{
			Transform * temp = new Transform();
			addChild(temp);
			temp->M = glm::translate(glm::mat4(1.0f), glm::vec3( i * 5.0f, 0.0f, j * 5.0f ) );
			temp->addChild(robot);
		}
	}
	sphere = new OBJObject("../sphere.obj");
	sphere->k_a = glm::vec3(1.0f);
	sphere->k_s = glm::vec3(1.0f);
	sphere->k_d = glm::vec3(1.0f);
	sphere->shininess = 1.0f;

	inside = std::vector<bool>(children.size(), true);
}

ArmyOfRobot::~ArmyOfRobot()
{
	if (children.size() > 0)
	{
		delete children[0];
	}
	for (int i = 1; i < children.size(); i++)
	{
		((Transform *)children[i])->removeChild(0);
		delete children[i];
	}
	delete sphere;
}

void ArmyOfRobot::drawBoundingSphere(GLuint shaderProgram, glm::mat4 C)
{
	// draw the bounding sphere here..?
	glm::mat4 model = C * M;
	for (int i = 0; i < children.size(); i++)
	{
		if (inside[i])
		{
			glm::mat4 child_model = model * ((Transform *)children[i])->M;
			child_model = glm::scale(child_model, glm::vec3(radius));
			sphere->toWorld = child_model;
			sphere->draw(shaderProgram);
		}
	}
}

void ArmyOfRobot::cull(std::vector<std::pair< glm::vec3, glm::vec3 > > listOfPlanes, glm::mat4 C, glm::mat4 P, glm::mat4 V)
{
	// find out which models to cull
	glm::mat4 model = C * M;

	// find the d value of each plane (d = pn)
	std::vector<float> d;
	for (int i = 0; i < listOfPlanes.size(); i++)
	{
		d.push_back(glm::dot(listOfPlanes[i].first, listOfPlanes[i].second));
	}

	// find the dist = xn - pn for each child for each plane, and check if the child is outside at least 1 plane
	for (int i = 0; i < children.size(); i++)
	{
		glm::mat4 child_model = model * ((Transform *) children[i])->M;
		glm::vec4 world_coord = child_model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 canonical_coord = P * V * world_coord;
		canonical_coord.x /= canonical_coord.w;
		canonical_coord.y /= canonical_coord.w;

		for (int j = 0; j < listOfPlanes.size(); j++)
		{
			float xn = 0;
			float r = radius;
			if (j < 2)
			{
				xn = glm::dot(glm::vec3(world_coord), listOfPlanes[j].second);
			}
			else
			{
				xn = glm::dot(glm::vec3(canonical_coord), listOfPlanes[j].second);
				r /= canonical_coord.w;
			}
			float dist = xn - d[j];
			//if (j == 0) std::cout << "dist from far plane = " << dist << " while z = " << camera_coord.w << std::endl;
			if (dist > r)
			{
				inside[i] = false;
				break;
			}
			else
			{
				inside[i] = true;
			}
		}
	}
}

void ArmyOfRobot::draw(GLuint shaderProgram, glm::mat4 C)
{
	glm::mat4 newMat = C * M;
	int count = 0;
	for (int i = 0; i < inside.size(); i++)
	{
		if (inside[i])
		{
			children[i]->draw(shaderProgram, newMat);
			count++;
		}
	}
	//if (rand() % 100 > 90)
	std::cout << "Display " << count << " androids out of " << army_size << std::endl;
}

World::World()
{
	M = glm::mat4(1.0f);
}

World::~World()
{
	for (auto child : children)
	{
		delete child;
	}
}

void World::update()
{
	for (auto child : children)
	{
		child->update();
	}
}
bool World::changeCPPosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V)
{
	return ((ConnectedCurves *)children[0])->changeCPPosition(width, height, x_prev, y_prev, x, y, P, V, M);
}
void World::continueChangePosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V)
{
	((ConnectedCurves *)children[0])->continueChangePosition(width, height, x_prev, y_prev, x, y, P, V, M);
}
void World::reset()
{
	((ConnectedCurves *)children[0])->reset();
}
void World::changeCamera(glm::mat4 & V, glm::vec3 & cam_pos, glm::vec3 & cam_look_at, glm::vec3 & cam_up)
{
	((ConnectedCurves *)children[0])->changeCamera(V, cam_pos, cam_look_at, cam_up);
}
void World::turnFrictionOn()
{
	((ConnectedCurves *)children[0])->turnFrictionOn();
}
//
//void World::drawBoundingSphere(GLuint shaderprogram, glm::mat4 C)
//{
//	((ArmyOfRobot *)children[0])->drawBoundingSphere(shaderprogram, C * M);
//}
//
//void World::cull(std::vector<std::pair< glm::vec3, glm::vec3 > > listOfPlanes, glm::mat4 C, glm::mat4 P, glm::mat4 V)
//{
//	((ArmyOfRobot *)children[0])->cull(listOfPlanes, C * M, P, V);
//}

BenzierCurve::BenzierCurve(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
	cp1 = p1;
	cp2 = p2;
	cp3 = p3;
	cp4 = p4;

	float t = 0;
	int segments = 200;

	glm::vec3 prev_p;
	glm::vec3 curr_p = findPoint(t);
	t += 1.0f / (float)segments;
	float prev_pos = 0.0f;

	for (; t <= 1.0; t += 1.0f / (float)segments)
	{
		prev_p = curr_p;
		curr_p = findPoint(t);
		LineSegment * temp = new LineSegment(prev_p, curr_p);
		temp->pos = prev_pos + temp->getLength();
		prev_pos = temp->pos;
		addChild(temp);
	}
	total_length = prev_pos;
}

BenzierCurve::~BenzierCurve()
{
	for (auto child : children)
	{
		delete child;
	}
}

void BenzierCurve::update()
{

}

void BenzierCurve::changeControlPoints(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4)
{
	cp1 = p1;
	cp2 = p2;
	cp3 = p3;
	cp4 = p4;

	float t = 0;
	int segments = 200;

	glm::vec3 prev_p;
	glm::vec3 curr_p = findPoint(t);
	t += 1.0f / (float)segments;
	float prev_pos = 0.0f;

	int i = 0;
	for (; t <= 1.0; t += 1.0f / (float)segments)
	{
		prev_p = curr_p;
		curr_p = findPoint(t);		
		LineSegment * temp = ((LineSegment *)children[i]);
		temp->pos = prev_pos + temp->getLength();
		prev_pos = temp->pos;
		temp->changePoints(prev_p, curr_p);
		i++;
	}
	total_length = prev_pos;
}

glm::vec3 BenzierCurve::findPoint(float t)
{
	glm::mat4 G_bez = { { cp1.x, cp1.y, cp1.z, 1.0f },{ cp2.x, cp2.y, cp2.z, 1.0f },{ cp3.x, cp3.y, cp3.z, 1.0f },{ cp4.x, cp4.y, cp4.z, 1.0f } };
	glm::vec4 T(t*t*t, t*t, t, 1);
	return glm::vec3(G_bez * B_bez * T);
}

glm::vec3 BenzierCurve::findPointByLength(float l)
{
	int i;
	for (i = 0; i < children.size(); i++)
	{
		if (((LineSegment *)children[i])->pos >= l)
			break;
	}

	if (i == 200) return findPoint(1.0f);
	LineSegment * temp = ((LineSegment *)children[i]);
	float ratio = (l - temp->pos) / temp->getLength();
	return findPoint( ((float)i+ratio) / 200.0f );
}

void BenzierCurve::draw(GLuint shaderProgram, glm::mat4 C)
{
	glm::vec3 color;
	if (accelerator)
	{
		color = glm::vec3(0.3f, 0.7f, 0.5f);
	}
	else
	{
		color = glm::vec3(0.5f, 0.3f, 0.7f);
	}

	for (auto child : children)
	{
		((LineSegment *)child)->draw(shaderProgram, C, color);
	}
}

ConnectedCurves::ConnectedCurves()
{
	// Sphere for the control points
	sphere = new OBJObject("../sphere.obj");
	sphere->k_a = glm::vec3(1.0f);
	sphere->k_s = glm::vec3(1.0f);
	sphere->k_d = glm::vec3(1.0f);
	sphere->shininess = 1.0f;

	// Randomly defined control points
	controlPoints[0] = glm::vec3(0.916546, -20.9195, -21.7371);
	controlPoints[1] = glm::vec3(37.2376, 2.7088, -23.1564);
	controlPoints[2] = glm::vec3(24.5897, 14.0787, -6.43606);
	controlPoints[3] = glm::vec3(28.1573, 1.47434, 6.17459);
	controlPoints[4] = glm::vec3(1.10736, 8.10312, 9.1812);
	controlPoints[5] = glm::vec3(-14.6825, 6.03055, 22.3827);
	controlPoints[6] = glm::vec3(-22.1082, -3.23762, 17.692);
	controlPoints[7] = glm::vec3(-11.1055, -25.0987, 15.2765);
	controlPoints[8] = controlPoints[0];

	// Randomly declare the third points of each curve
	thirdPoints[0] = glm::vec3(39.0221, -13.5807, -22.3931);
	thirdPoints[1] = glm::vec3(26.403, 9.72008, -19.8183);
	thirdPoints[2] = glm::vec3(31.517, 4.09444, 2.54825);
	thirdPoints[3] = glm::vec3(8.40623, 3.95971, 22.8377);
	thirdPoints[4] = glm::vec3(-24.6825, 11.0306, 15.3827);
	thirdPoints[5] = glm::vec3(-10.6008, -1.84264, 9.78952);
	thirdPoints[6] = glm::vec3(-17.6686, -21.7835, 14.2319);
	thirdPoints[7] = glm::vec3(10.9302, -33.02, 3.45382);

	// Find the second points using the C1 continuity
	secondPoints[0] = controlPoints[0] + (controlPoints[0] - thirdPoints[7]);
	for (int i = 1; i < numCurves; i++)
	{
		secondPoints[i] = controlPoints[i] + (controlPoints[i] - thirdPoints[i-1]);
	}

	// Find the controlLines 
	controlLines.push_back(new LineSegment(thirdPoints[7], secondPoints[0]));
	for (int i = 1; i < numCurves; i++)
	{
		controlLines.push_back(new LineSegment(thirdPoints[i-1], secondPoints[i]));
	}

	// Draw the curves
	for (int i = 0; i < numCurves; i++)
	{
		BenzierCurve * curve = new BenzierCurve(controlPoints[i], secondPoints[i], thirdPoints[i], controlPoints[i + 1]);
		addChild(curve);
		curves.push_back(curve);
	}

	total_lengths.push_back(curves[0]->total_length);
	for (int i = 1; i < curves.size(); i++)
	{
		total_lengths.push_back(total_lengths[i - 1] + curves[i]->total_length);
	}

	curr_pos = findHighestPosition();
	y_offset = findLowestPoint();
	sphere_pos = findPoint(curr_pos);
	starting_energy = 0.5f * 1.0f * curr_speed * curr_speed + (findY(curr_pos)) * 1.0f;
	total_energy = starting_energy;
}

ConnectedCurves::~ConnectedCurves()
{
	for (auto child : children)
	{
		delete child;
	}
}

void ConnectedCurves::update()
{
	static bool just_flipped = false;

	prev_pos = curr_pos;
	curr_pos = normalize(curr_pos + curr_speed * delta_t);
	sphere_pos = findPoint(curr_pos);

	if (friction)
	{
		// Add energy (accelerating) in a certain curve
		if (curr_pos > total_lengths[4] && curr_pos < total_lengths[5])
		{
			total_energy += 1.0f;
		}
		else
		{
			float temp = total_energy - 0.08f * glm::length(findPoint(curr_pos) - findPoint(prev_pos));
			if (temp - findY(curr_pos) > 0)
			{
				// Friction causes loss of energy proportional to the friction
				total_energy = temp;
			}
		}
	}

	if (total_energy - (findY(curr_pos)) > 0.0f)
	{
		curr_speed = sqrt( 2.0f * (total_energy - (findY(curr_pos))) );
		if (flipped) curr_speed = -1.0f * curr_speed;
		just_flipped = false;
	}
	else if (total_energy - (findY(curr_pos)) <= 0.0f)
	{
		if (!just_flipped) {
			flipped = !flipped;
			just_flipped = true;
		}

		curr_speed = sqrt( 2.0f * ((findY(curr_pos)) - total_energy) );
		if (flipped) curr_speed = -1.0f * curr_speed;
	}

	// static friction
	if (friction && (total_energy - findY(curr_pos) < -8.0f * glm::length(findPoint(curr_pos) - findPoint(prev_pos))))
	{
		total_energy = findY(curr_pos) - delta_t * curr_speed * curr_speed * 0.5f;
		curr_speed = 0.0f;
	}

	//if (abs(curr_speed) < 0.01f)
	//{
	//	curr_speed = sin(LineSegment(findPoint(curr_pos), findPoint(prev_pos)).angle);
	//}
}

void ConnectedCurves::reset()
{
	curr_speed = initial_speed;
	curr_pos = findHighestPosition();
	starting_energy = 0.5f * 1.0f * curr_speed * curr_speed + findY(curr_pos) * 1.0f;
	total_energy = starting_energy;
}

void ConnectedCurves::changeCamera(glm::mat4 & V, glm::vec3 & cam_pos, glm::vec3 & cam_look_at, glm::vec3 &cam_up)
{
	cam_pos = findPoint(curr_pos);
	if (!flipped)
		cam_look_at = findPoint(curr_pos + 0.01f);
	else
		cam_look_at = findPoint(curr_pos - 0.01f);

	cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

void ConnectedCurves::turnFrictionOn()
{
	friction = !friction;
	curves[5]->accelerator = !curves[5]->accelerator;
}

float ConnectedCurves::findHighestPosition()
{
	float pos = 0.0f;
	float highest_Y = findY(pos);
	float highest_pos = pos;
	
	for (; pos < total_lengths[numCurves-1]; pos += 0.001f)
	{
		float curr_Y = findY(pos);
		if (curr_Y > highest_Y)
		{
			highest_Y = curr_Y;
			highest_pos = pos;
		}
	}

	return highest_pos;
}

float ConnectedCurves::findLowestPoint()
{
	float pos = 0.0f;
	float lowest_Y = findY(pos);
	float lowest_pos = pos;

	for (; pos < total_lengths[numCurves - 1]; pos += 0.001f)
	{
		float curr_Y = findY(pos);
		if (curr_Y < lowest_Y)
		{
			lowest_Y = curr_Y;
			lowest_pos = pos;
		}
	}

	return lowest_Y;
}

float ConnectedCurves::findY(float pos)
{
	return findPoint(pos).y;
}

float ConnectedCurves::normalize(float pos)
{
	float normalized_pos = pos;
	if (pos > total_lengths[numCurves - 1])
	{
		normalized_pos -= floor(pos / total_lengths[numCurves - 1]) * total_lengths[numCurves - 1];
	}
	else if (pos < 0.0f)
	{
		while (normalized_pos < 0)
		{
			normalized_pos += total_lengths[numCurves - 1];
		}
	}
	return normalized_pos;
}

glm::vec3 ConnectedCurves::findPoint(float pos)
{
	float normalized_pos = normalize(pos);

	int index = 0;
	for (; index < numCurves; index++)
	{
		if (normalized_pos < total_lengths[index])
		{
			break;
		}
	}

	if (index == 0) return curves[index]->findPointByLength( normalized_pos );
	return curves[index]->findPointByLength( normalized_pos - total_lengths[index-1] );
}

bool ConnectedCurves::changeCPPosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
	int currIndex = -1;
	int whichVector = -1;
	float currZ = 0.0f;
	glm::mat4 toCube = P * V * M;
	glm::mat4 toCam = V * M;

	float c_x_prev = ((float)(x_prev / width))*2.0f - 1.0f;
	float c_x = ((float)(x / width))*2.0f - 1.0f;
	float c_y_prev = -1.0f * ( ((float)(y_prev / height))*2.0f - 1.0f );
	float c_y = -1.0f * ( ((float)(y / height))*2.0f - 1.0f );

	for (int i = 0; i < numCurves; i++)
	{
		glm::vec4 cube_coord = toCube * glm::vec4(controlPoints[i], 1.0f);
		float cp_x = cube_coord.x / cube_coord.w;
		float cp_y = cube_coord.y / cube_coord.w;

		float dist = sqrt( (cp_x-c_x_prev)*(cp_x - c_x_prev) + (cp_y - c_y_prev)*(cp_y - c_y_prev) );
		if (dist < 0.6f / cube_coord.w)
		{
			float thisZ = (toCam * glm::vec4(controlPoints[i], 1.0f)).z;

			if (currIndex == -1)
			{
				if (thisZ < 0.0f)
				{
					currIndex = i;
					currZ = thisZ;
					whichVector = 0;
				}
			}
			else if(thisZ > currZ)
			{
				currIndex = i;
				currZ = thisZ;
				whichVector = 0;
			}
		}
	}

	for (int i = 0; i < numCurves; i++)
	{
		glm::vec4 cube_coord = toCube * glm::vec4(secondPoints[i], 1.0f);
		float cp_x = cube_coord.x / cube_coord.w;
		float cp_y = cube_coord.y / cube_coord.w;

		float dist = sqrt((cp_x - c_x_prev)*(cp_x - c_x_prev) + (cp_y - c_y_prev)*(cp_y - c_y_prev));
		if (dist < 0.6f / cube_coord.w)
		{
			float thisZ = (toCam * glm::vec4(secondPoints[i], 1.0f)).z;

			if (currIndex == -1)
			{
				if (thisZ < 0.0f)
				{
					currIndex = i;
					currZ = thisZ;
					whichVector = 1;
				}
			}
			else if (thisZ > currZ)
			{
				currIndex = i;
				currZ = thisZ;
				whichVector = 1;
			}
		}
	}

	for (int i = 0; i < numCurves; i++)
	{
		glm::vec4 cube_coord = toCube * glm::vec4(thirdPoints[i], 1.0f);
		float cp_x = cube_coord.x / cube_coord.w;
		float cp_y = cube_coord.y / cube_coord.w;

		float dist = sqrt((cp_x - c_x_prev)*(cp_x - c_x_prev) + (cp_y - c_y_prev)*(cp_y - c_y_prev));
		if (dist < 0.6f / cube_coord.w)
		{
			float thisZ = (toCam * glm::vec4(thirdPoints[i], 1.0f)).z;

			if (currIndex == -1)
			{
				if (thisZ < 0.0f)
				{
					currIndex = i;
					currZ = thisZ;
					whichVector = 2;
				}
			}
			else if (thisZ > currZ)
			{
				currIndex = i;
				currZ = thisZ;
				whichVector = 2;
			}
		}
	}

	prev_currIndex = currIndex;
	prev_whichVector = whichVector;

	if (currIndex != -1)
		continueChangePosition(width, height, x_prev, y_prev, x, y, P, V, M);

	return currIndex != -1;
}

void ConnectedCurves::continueChangePosition(int width, int height, double x_prev, double y_prev, double x, double y, glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
	int currIndex = prev_currIndex;
	int whichVector = prev_whichVector;
	float currZ = 0.0f;
	glm::mat4 toCam = V * M;

	float c_x_prev = ((float)(x_prev / width))*2.0f - 1.0f;
	float c_x = ((float)(x / width))*2.0f - 1.0f;
	float c_y_prev = -1.0f * (((float)(y_prev / height))*2.0f - 1.0f);
	float c_y = -1.0f * (((float)(y / height))*2.0f - 1.0f);

	float delta_x = c_x - c_x_prev;
	float delta_y = c_y - c_y_prev;
	glm::mat4 toObject = glm::inverse(toCam);

	// What to do with control points?
	if (whichVector == 0)
	{
		glm::vec4 cam_coord = toCam * glm::vec4(controlPoints[currIndex], 1.0f);
		cam_coord.x -= delta_x * (cam_coord.z * (float)height/ (float)width) ;
		cam_coord.y -= delta_y * (cam_coord.z * (float)height / (float)width) ;

		glm::vec3 temp = controlPoints[currIndex];
		controlPoints[currIndex] = glm::vec3(toObject * cam_coord);
		temp = controlPoints[currIndex] - temp;

		if (currIndex == 0) controlPoints[8] = controlPoints[0];

		int prevIndex = currIndex - 1;
		if (prevIndex == -1) prevIndex = numCurves - 1;

		thirdPoints[prevIndex] += temp;
		secondPoints[currIndex] += temp;
		controlLines[currIndex]->changePoints(thirdPoints[prevIndex], secondPoints[currIndex]);

		curves[prevIndex]->changeControlPoints(controlPoints[prevIndex], secondPoints[prevIndex], thirdPoints[prevIndex], controlPoints[currIndex]);
		curves[currIndex]->changeControlPoints(controlPoints[currIndex], secondPoints[currIndex], thirdPoints[currIndex], controlPoints[ currIndex+1]);
	}
	// What to do with secondPoints ?
	else if (whichVector == 1)
	{
		glm::vec4 cam_coord = toCam * glm::vec4(secondPoints[currIndex], 1.0f);
		cam_coord.x -= delta_x * (cam_coord.z * (float)height / (float)width);
		cam_coord.y -= delta_y * (cam_coord.z * (float)height / (float)width);
		secondPoints[currIndex] = glm::vec3(toObject * cam_coord);

		int prevIndex = currIndex - 1;
		if (prevIndex == -1) prevIndex = numCurves - 1;

		thirdPoints[prevIndex] = controlPoints[currIndex] + (controlPoints[currIndex] - secondPoints[currIndex]);
		controlLines[currIndex]->changePoints(thirdPoints[prevIndex], secondPoints[currIndex]);

		curves[prevIndex]->changeControlPoints(controlPoints[prevIndex], secondPoints[prevIndex], thirdPoints[prevIndex], controlPoints[currIndex]);
		curves[currIndex]->changeControlPoints(controlPoints[currIndex], secondPoints[currIndex], thirdPoints[currIndex], controlPoints[currIndex + 1]);
	}
	// what to do with thirdPoints ?
	else if (whichVector == 2)
	{
		glm::vec4 cam_coord = toCam * glm::vec4(thirdPoints[currIndex], 1.0f);
		cam_coord.x -= delta_x * (cam_coord.z * (float)height / (float)width);
		cam_coord.y -= delta_y * (cam_coord.z * (float)height / (float)width);
		thirdPoints[currIndex] = glm::vec3(toObject * cam_coord);

		int nextIndex = currIndex + 1;
		if (nextIndex == numCurves) nextIndex = 0;

		secondPoints[nextIndex] = controlPoints[nextIndex] + (controlPoints[nextIndex] - thirdPoints[currIndex]);
		controlLines[nextIndex]->changePoints(thirdPoints[currIndex], secondPoints[nextIndex]);

		curves[currIndex]->changeControlPoints(controlPoints[currIndex], secondPoints[currIndex], thirdPoints[currIndex], controlPoints[nextIndex]);
		curves[nextIndex]->changeControlPoints(controlPoints[nextIndex], secondPoints[nextIndex], thirdPoints[nextIndex], controlPoints[nextIndex+1]);
	}

	float temp_curr = curr_pos;
	float temp_prev = prev_pos;
	std::vector<float> temp_lengths (total_lengths);


	total_lengths[0] = curves[0]->total_length;
	for (int i = 1; i < curves.size(); i++)
	{
		total_lengths[i] = total_lengths[i - 1] + curves[i]->total_length;
	}

	curr_pos = findNewPos(temp_curr, temp_lengths, total_lengths);
	prev_pos = findNewPos(temp_prev, temp_lengths, total_lengths);
}

float ConnectedCurves::findNewPos(float curr, std::vector<float> prev_l, std::vector<float> curr_l)
{
	int index = 0;
	for (; index < numCurves; index++)
	{
		if (curr < prev_l[index])
		{
			break;
		}
	}

	std::cerr << index << std::endl;
	float ratio;
	if (index == 0)
	{
		ratio = curr / prev_l[index];
		return normalize(ratio * curr_l[index]);
	}
	else
	{
		ratio = (curr - prev_l[index - 1]) / (prev_l[index] - prev_l[index - 1]);
		return normalize(curr_l[index-1] + ratio * (curr_l[index] - curr_l[index - 1]));
	}

	
}

void ConnectedCurves::draw(GLuint shaderProgram, glm::mat4 C)
{
	Transform::draw(shaderProgram, C);
	glm::mat4 newMat = C * M;
	newMat = glm::scale(newMat, glm::vec3(0.3f, 0.3f, 0.3f));
	
	// Draw the control points
	GLuint loc = glGetUniformLocation(shaderProgram, "draw_sphere");
	glUniform1i(loc, 1);

	for (int i = 0; i < numCurves; i++)
	{
		sphere->toWorld = glm::translate(glm::mat4(1.0f), controlPoints[i] ) * newMat;
		sphere->draw(shaderProgram);
	}

	glUniform1i(loc, 2);

	for (int i = 0; i < numCurves; i++)
	{
		sphere->toWorld = glm::translate(glm::mat4(1.0f), secondPoints[i]) * newMat;
		sphere->draw(shaderProgram);

		sphere->toWorld = glm::translate(glm::mat4(1.0f), thirdPoints[i]) * newMat;
		sphere->draw(shaderProgram);
	}

	glUniform1i(loc, 0);

	// Draw the lines of the Benzier handles
	loc = glGetUniformLocation(shaderProgram, "draw_line");
	glUniform1i(loc, 2);
	for (auto line : controlLines)
	{
		line->draw(shaderProgram, C * M, glm::vec3(0.3f,0.3,1.0f));
	}
	glUniform1i(loc, 0);

	// Draw the roller coaster
	loc = glGetUniformLocation(shaderProgram, "environment_map");
	glUniform1i(loc, 1);

	sphere->toWorld = glm::scale(C*M, glm::vec3(2.0f));
	sphere->toWorld = glm::translate(glm::mat4(1.0f), sphere_pos) * sphere->toWorld;
	sphere->draw(shaderProgram);

	glUniform1i(loc, 0);
}

void ConnectedCurves::printVertices()
{
	std::cerr << "Main control points: " << std::endl;
	for (auto vertex : controlPoints)
	{
		std::cerr << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
	}

	std::cerr << std::endl << "Third control points: " << std::endl;
	for (auto vertex : thirdPoints)
	{
		std::cerr << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
	}
}
