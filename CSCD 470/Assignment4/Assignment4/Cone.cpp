#include "Cone.h"

unsigned int cone_vao;

void createCone()
{
	int i, index = 0;
	float theta, thetaOne;;
	vec4 p1, p2, p3;

	vec4 points[NumVertices];
	vec3 normals[NumVertices];
	vec2 textures[NumVertices];

	GLuint indices[NumIndices];

	GLfloat steps = 360.0 / NumTriangles;

	vec4 red(1.0, 0.0, 0.0, 1.0);
	vec2 center(0.5, 0.5);

	//tip of cone
	points[0] = vec4(0.0, 1.0, 0.0, 1.0);
	normals[0] = vec3(0.0, 0.0, 0.0);
	textures[0] = vec2(0.0, 0.0);

	//base vertices
	for (i = 0; i < NumTriangles; ++i) {

		theta = i * steps;
		thetaOne = (i + 1) * steps;

		points[i + 1] = vec4(cos(radians(theta)), 0.0, sin(radians(theta)), 1.0);
		normals[i + 1] = vec3(0.0, 0.0, 0.0);
		textures[i + 1] = vec2(0.0, 0.0);

		if (i < (NumTriangles - 1)) {
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

	for (int i = 0; i < NumIndices; i += 3) {
		p1 = points[indices[i + 0]];
		p2 = points[indices[i + 1]];
		p3 = points[indices[i + 2]];

		vec3 N = normalize(cross(vec3(p3 - p1), (vec3(p2 - p1))));

		normals[indices[i + 0]] += N; textures[indices[i + 0]] = center;
		normals[indices[i + 1]] += N; textures[indices[i + 1]] = vec2((1 + cos(theta)) / 2.0, (1 + sin(theta)) / 2.0);
		normals[indices[i + 2]] += N; textures[indices[i + 2]] = vec2((1 + cos(thetaOne)) / 2.0, (1 + sin(thetaOne)) / 2.0);
	}

	for (int i = 0; i < NumVertices; i++) {
		normals[i] = normalize(vec3(normals[i]));
	}


	glGenVertexArrays(1, &cone_vao);
	glBindVertexArray(cone_vao);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // Vertex textures

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawCone() {
	glBindVertexArray(cone_vao);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
}