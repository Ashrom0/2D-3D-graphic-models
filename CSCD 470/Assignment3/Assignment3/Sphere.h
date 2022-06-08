#include <stdio.h> 
#include <GL/glew.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

#define STEP 15
#define NumVertices STEP*(STEP-1)+2
#define NumTriangles STEP*(STEP-1)*2
#define NumIndices NumTriangles*3

void createSphere();
void drawSphere();

extern unsigned int sphere_vao;