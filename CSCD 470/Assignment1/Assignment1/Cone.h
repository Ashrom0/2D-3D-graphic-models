#include <stdio.h>
#include <GL/glew.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

#define NumTriangles  30
#define NumVertices   NumTriangles + 1
#define NumIndices   NumTriangles*3

void createCone();
void drawCone();

extern	unsigned int cone_vao;
