#include "roof.h"

unsigned int pyramid_vao;

void createPyramid() {
	float side = 1.0f;

	const int numVertices = 12;
	vec4 roof_vertices[numVertices] = {
		// front triangle
		{0.0,  side * 3, 0.0, 1.0f},    // index 0
		{-side, 0.0, side, 1.0f},      // index 1
		{side, 0.0, side, 1.0f},      // index 2
		// right triangle
		{0.0,  side * 3, 0.0, 1.0f},     //index 3
		{side, 0.0, side, 1.0f},       //index 4
		{side, 0.0, -side, 1.0f},     //index 5
		// back triangle
		{0.0,  side * 3, 0.0, 1.0f},     // index 6
		{side, 0.0, -side, 1.0f},      //index 7
		{-side, 0.0, -side, 1.0f},      //index 8
		//left triangle
		{ 0.0,  side * 3, 0.0, 1.0f},     // index 9
		{-side, 0.0, -side, 1.0f},     // index 10
		{ -side, 0.0, side, 1.0f},      // index 11
	};

	vec3 P0 = vec3(roof_vertices[0]);
	vec3 P1 = vec3(roof_vertices[1]);
	vec3 P2 = vec3(roof_vertices[2]);

	vec3 normalF = normalize(cross(P1 - P0, P2 - P0));

	vec3 P3 = vec3(roof_vertices[5]);
	vec3 normalR = normalize(cross(P2 - P0, P3 - P0));

	vec3 P4 = vec3(roof_vertices[8]);
	vec3 normalB = normalize(cross(P3 - P0, P4 - P0));

	vec3 normalL = normalize(cross(P4 - P0, P1 - P0));


	vec3 roof_normals[numVertices] = {
		{normalF}, //front face
		{normalF},
		{normalF},
		{normalR}, // right face
		{normalR},
		{normalR},
		{normalB}, //back face
		{normalB},
		{normalB},
		{normalL}, // left face
		{normalL},
		{normalL},
	};

	vec2 roof_textures[numVertices] = {
		{0.5, 0.5},  //front
		{0.0, 0.0},
		{1.0, 0.0},

		{0.5, 0.5},
		{1.0, 0.0},
		{1.0, 1.0},

		{0.5, 0.5},
		{1.0, 1.0},
		{0.0, 1.0},

		{0.5, 0.5},
		{0.0, 1.0},
		{0.0, 0.0},
	};

	glGenVertexArrays(1, &pyramid_vao);
	glBindVertexArray(pyramid_vao);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roof_vertices), roof_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roof_normals), roof_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roof_textures), roof_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void drawPyramid() {
	glBindVertexArray(pyramid_vao);
	glDrawArrays(GL_TRIANGLES, 0, 12);
}