#include "Plane.h"

unsigned int plane_vao;

void createPlane()
{
	float side = 1.0f;

	GLfloat plane_vertices[] = { -side, 0.0, side, 1.0f,  // v0,v1,v2,v3 (front)
							   side, 0.0, side, 1.0f,
							   side, 0.0, -side, 1.0f,
								-side, 0.0, -side, 1.0f,

								 };


	GLfloat plane_colors[] = { 1.0f, 0.0f, 1.0f, 1.0f, // v0,v1,v2,v3 (front)
								0.0f, 1.0f, 1.0f, 1.0f,
								0.0f, 0.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 0.0f, 1.0f,

								 };


	GLushort plane_indices[] = { 0, 2, 1, 0, 3, 2 };

	glGenVertexArrays(1, &plane_vao);
	glBindVertexArray(plane_vao);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	//colors
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_colors), plane_colors, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex position

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_indices), plane_indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawPlane() {
	glBindVertexArray(plane_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}
