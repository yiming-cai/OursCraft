#include "Geometry.h"

void Geometry::draw(glm::mat4 C) {

	glUseProgram(shader);
	glm::mat4 model = C * toWorld;
	glm::vec3 cam_pos = glm::vec3(V[0][3],V[1][3],V[2][3]);

	//glm::vec4 temp = glm::vec4(vertices[6], vertices[7], vertices[8], 1.0f);
	uProjection = glGetUniformLocation(shader, "projection");
	uModel = glGetUniformLocation(shader, "model");
	uView = glGetUniformLocation(shader, "view");
	GLuint sel = glGetUniformLocation(shader, "selected");
	GLuint uCam = glGetUniformLocation(shader, "cam_pos");
	if (haveTexture)
	{
		GLuint haveT = glGetUniformLocation(shader, "haveTexture");
		glUniform1i(haveT,haveTexture);
	}
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &V[0][0]);
	glUniform3fv(uCam, 1, &cam_pos[0]);
	glUniform1i(sel, selected);

	// optimization: to avoid buffering same vao all the time
	// !!note, don't use this unless you are drawing the same object over and over again
	// Basically, it is expensive to keep on binding and unbinding VAO, so we reduce that
	if (!drawingCube)
	{
		glBindVertexArray(VAO);
	}
	else if (!bindedCubeVAO)
	{
		glBindVertexArray(VAO);
		bindedCubeVAO = true;
	}
	if (haveTexture) glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, verticesLen / 3);
	//glBindVertexArray(0);

	if (haveTexture)
	{
		GLuint haveT = glGetUniformLocation(shader, "haveTexture");
		glUniform1i(haveT, 0);
	}
	selected = 0;
}

void Geometry::update() {

}