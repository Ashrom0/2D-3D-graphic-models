#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "Parasol.h"

using namespace glm;

unsigned int parasol_vao;

void createParasol() {
	GLfloat parasol_vertices[] = { 0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(0.0)), 0.0, 2.0 * sin(radians(0.0)), 1.0, // index '1'
					   2.0 * cos(radians(60.0)), 0.0, 2.0 * sin(radians(60.0)), 1.0, // index '2'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(60.0)), 0.0, 2.0 * sin(radians(60.0)), 1.0, // index '2'
					   2.0 * cos(radians(120.0)), 0.0, 2.0 * sin(radians(120.0)), 1.0, // index '3'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(120.0)), 0.0, 2.0 * sin(radians(120.0)), 1.0, // index '3'
					   2.0 * cos(radians(180.0)), 0.0, 2.0 * sin(radians(180.0)), 1.0, // index '4'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(180.0)), 0.0, 2.0 * sin(radians(180.0)), 1.0, // index '4'
					   2.0 * cos(radians(240.0)), 0.0, 2.0 * sin(radians(240.0)), 1.0, // index '5'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(240.0)), 0.0, 2.0 * sin(radians(240.0)), 1.0, // index '5'
					   2.0 * cos(radians(300.0)), 0.0, 2.0 * sin(radians(300.0)), 1.0, // index '6'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(300.0)), 0.0, 2.0 * sin(radians(300.0)), 1.0, // index '6'
					   2.0 * cos(radians(0.0)), 0.0, 2.0 * sin(radians(0.0)), 1.0, // index '1'
	};

	GLfloat update_vertices[] = { 0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(0.0)), -3.0, 2.0 * sin(radians(0.0)), 1.0, // index '1'
					   2.0 * cos(radians(60.0)), -3.0, 2.0 * sin(radians(60.0)), 1.0, // index '2'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(60.0)), -3.0, 2.0 * sin(radians(60.0)), 1.0, // index '2'
					   2.0 * cos(radians(120.0)), -3.0, 2.0 * sin(radians(120.0)), 1.0, // index '3'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(120.0)), -3.0, 2.0 * sin(radians(120.0)), 1.0, // index '3'
					   2.0 * cos(radians(180.0)), -3.0, 2.0 * sin(radians(180.0)), 1.0, // index '4'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(180.0)), -3.0, 2.0 * sin(radians(180.0)), 1.0, // index '4'
					   2.0 * cos(radians(240.0)), -3.0, 2.0 * sin(radians(240.0)), 1.0, // index '5'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(240.0)), -3.0, 2.0 * sin(radians(240.0)), 1.0, // index '5'
					   2.0 * cos(radians(300.0)), -3.0, 2.0 * sin(radians(300.0)), 1.0, // index '6'

					   0.0, 0.5, 0.0, 1.0,                                     //index '0'
					   2.0 * cos(radians(300.0)), -3.0, 2.0 * sin(radians(300.0)), 1.0, // index '6'
					   2.0 * cos(radians(0.0)), -3.0, 2.0 * sin(radians(0.0)), 1.0, // index '1'
	};

	GLfloat parasol_colors[] = {
		1.0,0.0,0.0,1.0,
		1.0,0.0,0.0,1.0, //red
		1.0,0.0,0.0,1.0,

		1.0,1.0,0.0,1.0,
		1.0,1.0,0.0,1.0, //yellow
		1.0,1.0,0.0,1.0,

		0.0,1.0,0.0,1.0,
		0.0,1.0,0.0,1.0, //green
		0.0,1.0,0.0,1.0,

		0.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0, //blue
		0.0,0.0,1.0,1.0,

		0.0,1.0,1.0,1.0,
		0.0,1.0,1.0,1.0, //cyan
		0.0,1.0,1.0,1.0,

		1.0,0.0,1.0,1.0,
		1.0,0.0,1.0,1.0, //magenta
		1.0,0.0,1.0,1.0,

	};

	GLushort parasol_indices[] = { 0, 1, 2,
					   3, 4, 5,
					   6, 7, 8,
					   9, 10, 11,
					   12, 13, 14,
					   15, 16, 17,
	};

	glGenVertexArrays(1, &parasol_vao);
	glBindVertexArray(parasol_vao);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(parasol_vertices), parasol_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(parasol_colors), parasol_colors, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(parasol_indices), parasol_indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawParasol() {
	glBindVertexArray(parasol_vao);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
}