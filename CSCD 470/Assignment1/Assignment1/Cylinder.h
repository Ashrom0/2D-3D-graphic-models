#include <stdio.h> 
#include <GL/glew.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

#define STEP 15
#define CylinderNumVertices 2*STEP
#define CylinderNumTriangles 2*STEP
#define CylinderNumIndices 3*CylinderNumTriangles

void createCylinder();
void drawCylinder();

extern unsigned int cylinder_vao;
