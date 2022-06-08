#include "Sphere.h" 

unsigned int sphere_vao;

vec4 points[SphereNumVertices];
vec3 normals[SphereNumVertices];
GLuint indices[SphereNumIndices];
vec2 textures[SphereNumVertices];
vec4 tangents[SphereNumVertices];

void updateVertexTangents(vec4* points, vec3* normals, vec4* tangents, int numvertices, int numindices, GLuint* indices, vec2* tex) {
	vec3* tan1 = (vec3*)malloc(sizeof(vec3) * SphereNumIndices);
	vec3* tan2 = (vec3*)malloc(sizeof(vec3) * SphereNumIndices);

	for (int k = 0; k < SphereNumIndices; k++) {
		tan1[k] = vec3(0.0f, 0.0f, 0.0f);
		tan2[k] = vec3(0.0f, 0.0f, 0.0f);
	}

	// Compute the tangent vector
	for (int i = 0; i < SphereNumIndices; i += 3)
	{
		int index_0 = indices[i];
		int index_1 = indices[i + 1];
		int index_2 = indices[i + 2];

		vec3 p1 = vec3(points[index_0]);
		vec3 p2 = vec3(points[index_1]);
		vec3 p3 = vec3(points[index_2]);

		vec2 tc1 = tex[index_0];
		vec2 tc2 = tex[index_1];
		vec2 tc3 = tex[index_2];

		vec3 q1 = p2 - p1;
		vec3 q2 = p3 - p1;
		float s1 = tc2.x - tc1.x;
		float s2 = tc3.x - tc1.x;
		float t1 = tc2.y - tc1.y;
		float t2 = tc3.y - tc1.y;
		float r = 1.0f / (s1 * t2 - s2 * t1);

		glm::vec3 tangents1((t2 * q1.x - t1 * q2.x) * r, (t2 * q1.y - t1 * q2.y) * r, (t2 * q1.z - t1 * q2.z) * r);
		glm::vec3 tangents2((s1 * q2.x - s2 * q1.x) * r, (s1 * q2.y - s2 * q1.y) * r, (s1 * q2.z - s2 * q1.z) * r);

		tan1[index_0] += tangents1;
		tan1[index_1] += tangents1;
		tan1[index_2] += tangents1;
		tan2[index_0] += tangents2;
		tan2[index_1] += tangents2;
		tan2[index_2] += tangents2;
	}

	for (int i = 0; i < SphereNumVertices; ++i)
	{
		vec3 n = normals[i];
		vec3 t1 = tan1[i];
		vec3 t2 = tan2[i];

		// Gram-Schmidt orthogonalize
		tangents[i] = glm::vec4(glm::normalize(t1 - (glm::dot(n, t1) * n)), 0.0f);
		// Store handedness in w
		tangents[i].w = (glm::dot(glm::cross(n, t1), t2) < 0.0f) ? -1.0f : 1.0f;
	}

	free(tan1);
	free(tan2);
}

void createSphere() {

	int i = 0, j, index = 0;
	int indexOne, indexTwo, indexThree, indexFour;
	float theta, phi;
	//vec4 p1, p2, p3;
	float kPI = 3.1415926535897932384626433832795;

	


	for (double b = -STEP / 2.0; b <= STEP / 2.0; b++) {
		for (int a = 0; a <= STEP; a++) {
			phi = (1.0 * a / STEP) * 2 * kPI;
			theta = (1.0 * b / STEP) * kPI;

			points[i].x = cos(theta) * sin(phi);
			points[i].y = sin(theta);
			points[i].z = cos(theta) * cos(phi);
			points[i].w = 1.0f;

			normals[i] = normalize(vec3(points[i]));

			textures[i].x = 1.0 * a / STEP;
			textures[i].y = (1.0 * b / STEP) + 0.5;
			i++;
		}

	}

	for (int i = 1; i < (SphereNumVertices - STEP - 1); i += STEP + 1) {
		for (int j = i; j < (i + STEP); j++) {
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

	updateVertexTangents(points, normals, tangents, 4, 6, indices, textures);

	glGenVertexArrays(1, &sphere_vao);
	glBindVertexArray(sphere_vao);

	unsigned int handle[5];
	glGenBuffers(5, handle);

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

	glBindBuffer(GL_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)3, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);  // Vertex texture

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(0);
}

void drawSphere() {
	glBindVertexArray(sphere_vao);
	glDrawElements(GL_TRIANGLES, SphereNumIndices, GL_UNSIGNED_INT, 0);
}