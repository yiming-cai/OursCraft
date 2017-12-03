#pragma once
#include "Node.h"

class Transform :public Node {
protected:
	glm::mat4 M;
	std::vector<Node *> list;
public:
	Transform() {};
	Transform(int id,glm::mat4 M) {
		this->M = M;
		this->id = id;
	}
	void addChild(Node *s) {
		list.push_back(s);
	}
	bool removeChild(int id);
	void draw(glm::mat4 C);
	void update();

};