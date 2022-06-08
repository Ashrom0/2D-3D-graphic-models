#include "Disk.h"

unsigned int disk_vao;

void createDisk()
{
	int i, index = 0;
	float theta;
	vec4 p1, p2, p3;

	vec4 points[DiskNumVertices];
	vec4 colors[DiskNumVertices];
	vec3 normals[DiskNumVertices];

	GLuint indices[DiskNumIndices];

	GLfloat steps = 360.0 / DiskNumTriangles;

	vec4 red(1.0, 0.0, 0.0, 1.0);

	//tip of cone
	points[0] = vec4(0.0, 0.0, 0.0, 1.0); //only change from cone.cpp, to make it flat
	colors[0] = red;
	normals[0] = vec3(0.0, 0.0, 0.0);

	//base vertices
	for (i = 0; i < DiskNumTriangles; ++i) {

		theta = i * steps;

		points[i + 1] = vec4(cos(radians(theta)), 0.0, sin(radians(theta)), 1.0);
		normals[i + 1] = vec3(0.0, 0.0, 0.0);
		colors[i + 1] = red;

		if (i < (DiskNumTriangles - 1)) {
			indices[3 * i + 0] = 0;
			indices[3 * i + 1] = i + 1;
			indices[3 * i + 2] = i + 2;
		}
		else {
			indices[3 * i + 0] = 0;
			indices[3 * i + 1] = i + 1;
			indices[3 * i + 2] = 1;
		}
	}

	for (int i = 0; i < DiskNumIndices; i += 3) {
		vec4 p1 = points[indices[i + 0]];
		vec4 p2 = points[indices[i + 1]];
		vec4 p3 = points[indices[i + 2]];

		vec3 N = normalize(cross(vec3(p3 - p1), (vec3(p2 - p1))));

		normals[indices[i + 0]] += N;
		normals[indices[i + 1]] += N;
		normals[indices[i + 2]] += N;
	}

	for (int i = 0; i < DiskNumVertices; i++) {
		normals[i] = normalize(vec3(normals[i]));
	}


	glGenVertexArrays(1, &disk_vao);
	glBindVertexArray(disk_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position


	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawDisk() {
	glBindVertexArray(disk_vao);
	glDrawElements(GL_TRIANGLES, DiskNumIndices, GL_UNSIGNED_INT, 0);
}