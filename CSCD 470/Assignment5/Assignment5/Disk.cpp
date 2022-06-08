#include "Disk.h"

unsigned int disk_vao;

void createDisk()
{
	int i, index = 0;
	float theta, thetaOne;
	vec4 p1, p2, p3;

	vec4 points[DiskNumVertices];
	vec3 normals[DiskNumVertices];
	vec2 textures[DiskNumVertices];

	//GLuint indices[DiskNumIndices];

	GLfloat steps = 360.0 / DiskNumTriangles;

	vec4 red(1.0, 0.0, 0.0, 1.0);
	vec2 center(0.5, 0.5);

	//tip of cone
	points[0] = vec4(0.0, 1.0, 0.0, 1.0); //only change from cone.cpp, to make it flat
	normals[0] = vec3(0.0, 0.0, 0.0);
	textures[0] = vec2(0.0, 0.0);

	for (i = 0; i < DiskNumTriangles; ++i) {

		theta = i * steps;
		thetaOne = (i + 1) * steps;

		p1 = vec4(0.0, 0.0, 0.0, 1.0);
		p2 = vec4(cos(radians(theta)), 0.0, sin(radians(theta)), 1.0);
		p3 = vec4(cos(radians(thetaOne)), 0.0, sin(radians(thetaOne)), 1.0);

		vec3 normal = normalize(vec3((cross(vec3(p2 - p1), vec3(p3 - p1)))));


		points[index] = p1; normals[index] = normal; textures[index] = center; index++;
		points[index] = p2; normals[index] = normal; textures[index] = vec2((1 + cos(theta)) / 2.0, (1 + sin(theta)) / 2.0); index++;
		points[index] = p3; normals[index] = normal; textures[index] = vec2((1 + cos(thetaOne)) / 2.0, (1 + sin(thetaOne)) / 2.0); index++;

	}


	glGenVertexArrays(1, &disk_vao);
	glBindVertexArray(disk_vao);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position


	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // Vertex texture

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	glBindVertexArray(0);

}

void drawDisk() {
	glBindVertexArray(disk_vao);
	glDrawArrays(GL_TRIANGLES, 0, DiskNumVertices);
}