#include "Cylinder.h" 

unsigned int cylinder_vao;

void createCylinder()
{
	int i, index = 0;
	float theta;
	vec4 p1, p2, p3;

	vec4 points[CylinderNumVertices];
	vec3 normals[CylinderNumVertices];
	vec2 textures[CylinderNumVertices];

	GLuint indices[CylinderNumIndices];

	GLfloat intervals = 360.0 / STEP;

	//base vertices
	for (i = 0; i < STEP; i++) {
		theta = i * intervals;

		points[i] = vec4(sin(radians(theta)), -1.0, cos(radians(theta)), 1.0);
		points[i + STEP] = vec4(sin(radians(theta)), 1.0, cos(radians(theta)), 1.0);

		normals[i] = normalize(vec3(points[i].x, 0, points[i].z));
		normals[i + STEP] = normalize(vec3(points[i + STEP].x, 0, points[i + STEP].z));

		textures[i] = vec2((float)(i) / STEP, 1.0);
		textures[i + STEP] = vec2((float)(i) / STEP, 0.0);

		//colors[i] = red;
		//colors[i + STEP] = red;
	}

	for (i = 0; i < STEP; i++) {
		if (i == STEP - 1) {
			indices[index++] = i;
			indices[index++] = i + STEP;
			indices[index++] = 0;
			indices[index++] = i + STEP;
			indices[index++] = STEP;
			indices[index++] = 0;
		}
		else {
			indices[index++] = i;
			indices[index++] = i + STEP;
			indices[index++] = i + 1;
			indices[index++] = i + STEP;
			indices[index++] = i + 1 + STEP;
			indices[index++] = i + 1;
		}
	}

	glGenVertexArrays(1, &cylinder_vao);
	glBindVertexArray(cylinder_vao);

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
	glEnableVertexAttribArray(2);  // Vertex normal

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawCylinder() {
	glBindVertexArray(cylinder_vao);
	glDrawElements(GL_TRIANGLES, CylinderNumIndices, GL_UNSIGNED_INT, 0);
}
