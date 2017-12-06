#include "Geometry.h"


void Geometry::draw(glm::mat4 C) {

	glUseProgram(shader);
	glm::mat4 modelview = V * C * toWorld;
	//glm::vec4 temp = glm::vec4(vertices[6], vertices[7], vertices[8], 1.0f);
	uProjection = glGetUniformLocation(shader, "projection");
	uModelview = glGetUniformLocation(shader, "modelview");
	GLuint sel = glGetUniformLocation(shader, "selected");
	if (haveTexture)
	{
		GLuint haveT = glGetUniformLocation(shader, "haveTexture");
		glUniform1i(haveT,haveTexture);
	}
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniform1i(sel, selected);
	glBindVertexArray(VAO);
	if(haveTexture) glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	if (haveTexture)
	{
		GLuint haveT = glGetUniformLocation(shader, "haveTexture");
		glUniform1i(haveT, 0);
	}
	selected = 0;
}

void Geometry::update() {

}