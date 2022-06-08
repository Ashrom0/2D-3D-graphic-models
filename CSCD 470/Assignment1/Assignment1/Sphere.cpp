#include "Sphere.h" 

unsigned int sphere_vao;

void createSphere() {

	int i = 0, j, index = 0;
	float theta, phi;
	vec4 p1, p2, p3;
	float kPI = 3.1415926535897932384626433832795;

	vec4 points[NumVertices];
	vec4 colors[NumVertices];
	vec3 normals[NumVertices];
	GLuint indices[NumIndices];

	vec4 red(1.0, 0.0, 0.0, 1.0);

	for (double b = -STEP / 2.0; b <= STEP / 2.0; b++) {
		if (b == -STEP / 2.0 || b == STEP / 2.0) {
			theta = (1.0 * b / STEP) * kPI;

			points[i].x = 0;
			points[i].y = sin(theta);
			points[i].z = 0;
			points[i].w = 1.0f;
			i++;
		}
		else {
			for (int a = 0; a < STEP; a++) {
				phi = (1.0 * a / STEP) * 2 * kPI;
				theta = (1.0 * b / STEP) * kPI;

				points[i].x = cos(theta) * sin(phi);
				points[i].y = sin(theta);
				points[i].z = cos(theta) * cos(phi);
				points[i].w = 1.0f;
				i++;
			}
		}
	}


	for (int i = 0; i < NumVertices; i++) {
		normals[i] = normalize(vec3(points[i]));
	}

	// bottom (cone)
	for (i = 0; i < STEP; i++) {
		if (i == STEP - 1) {
			indices[index++] = i;
			indices[index++] = i + 1;
			indices[index++] = 1;
		}
		else {
			indices[index++] = i;
			indices[index++] = i + 1;
			indices[index++] = i + 2;
		}
	}

	//top (inverse cone) DONE
	for (i = NumVertices - STEP - 1; i < NumVertices - 1; i++) {
		if (i == NumVertices - 2) {
			indices[index++] = NumVertices - STEP - 1;
			indices[index++] = NumVertices - 1;
			indices[index++] = i;

		}
		else {
			indices[index++] = i;
			indices[index++] = i + 1;
			indices[index++] = NumVertices - 1;
		}
	}
	//middle (everything in between)
	for (i = 1; i < (NumVertices - STEP - 1); i += STEP) {
		for (j = i; j < (i + STEP); j++) {
			if (j == i + STEP - 1) {
				indices[index++] = j;
				indices[index++] = j + STEP;
				indices[index++] = i;
				indices[index++] = j + STEP;
				indices[index++] = i + STEP;
				indices[index++] = i;
			}
			else {
				indices[index++] = j;
				indices[index++] = j + STEP;
				indices[index++] = j + 1;
				indices[index++] = j + STEP;
				indices[index++] = j + 1 + STEP;
				indices[index++] = j + 1;
			}
		}
	}

	glGenVertexArrays(1, &sphere_vao);
	glBindVertexArray(sphere_vao);

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
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(0);
}

void drawSphere() {
	glBindVertexArray(sphere_vao);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
}