#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Object.h"

class Geometry:public Object {
protected:
	GLuint shader;
	int selected;
	int haveTexture;
	GLuint textureID;
	int disableLight = 0;

public:
	void draw(glm::mat4 C);
	void update();
	void setSelected() {
		selected = 1;
	}
	void toggleLight() { disableLight = (disableLight == 0 ? 1 : 0); }
};


#endif