#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

GLfloat vertices[] = { 0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(0.0)), 0.8 * sin(radians(0.0)), 0.0, 1.0, // index '1'
					   0.8 * cos(radians(22.50)), 0.8 * sin(radians(22.50)), 0.0, 1.0, // index '2'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(22.50)), 0.8 * sin(radians(22.50)), 0.0, 1.0, // index '2'
					   0.8 * cos(radians(45.0)), 0.8 * sin(radians(45.0)), 0.0, 1.0, // index '3'

					   0.0, 0.0, 1.0, 1.0,                                          //index '0'
					   0.8 * cos(radians(45.0)), 0.8 * sin(radians(45.0)), 0.0, 1.0, // index '3'
					   0.8 * cos(radians(67.5)), 0.8 * sin(radians(67.5)), 0.0, 1.0, // index '4'

					   0.0, 0.0, 1.0, 1.0,                                           //index '0'
					   0.8 * cos(radians(67.5)), 0.8 * sin(radians(67.5)), 0.0, 1.0, // index '4'
					   0.8 * cos(radians(90.0)), 0.8 * sin(radians(90.0)), 0.0, 1.0, // index '5'

					   0.0, 0.0, 1.0, 1.0,                                           //index '0'
					   0.8 * cos(radians(90.0)), 0.8 * sin(radians(90.0)), 0.0, 1.0, // index '5'
					   0.8 * cos(radians(112.5)), 0.8 * sin(radians(112.5)), 0.0, 1.0, // index '6'

					   0.0, 0.0, 1.0, 1.0,                                             //index '0'
					   0.8 * cos(radians(112.5)), 0.8 * sin(radians(112.5)), 0.0, 1.0, // index '6'
					   0.8 * cos(radians(135.0)), 0.8 * sin(radians(135.0)), 0.0, 1.0, // index '7'

					   0.0, 0.0, 1.0, 1.0,                                             //index '0'
					   0.8 * cos(radians(135.0)), 0.8 * sin(radians(135.0)), 0.0, 1.0, // index '7'
					   0.8 * cos(radians(157.5)), 0.8 * sin(radians(157.5)), 0.0, 1.0, // index '8'

					   0.0, 0.0, 1.0, 1.0,                                              //index '0'
					   0.8 * cos(radians(157.5)), 0.8 * sin(radians(157.5)), 0.0, 1.0, // index '8'
					   0.8 * cos(radians(180.0)), 0.8 * sin(radians(180.0)), 0.0, 1.0, // index '9'
};

GLfloat vertices_update[] = { 0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(50.0)), 0.8 * sin(radians(50.0)), 0.0, 1.0, // index '1'
					   0.8 * cos(radians(60.0)), 0.8 * sin(radians(60.0)), 0.0, 1.0, // index '2'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(60.0)), 0.8 * sin(radians(60.0)), 0.0, 1.0, // index '2'
					   0.8 * cos(radians(70.0)), 0.8 * sin(radians(70.0)), 0.0, 1.0, // index '3'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(70.0)), 0.8 * sin(radians(70.0)), 0.0, 1.0, // index '3'
					   0.8 * cos(radians(80.0)), 0.8 * sin(radians(80.0)), 0.0, 1.0, // index '4'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(80.0)), 0.8 * sin(radians(80.0)), 0.0, 1.0, // index '4'
					   0.8 * cos(radians(90.0)), 0.8 * sin(radians(90.0)), 0.0, 1.0, // index '5'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(90.0)), 0.8 * sin(radians(90.0)), 0.0, 1.0, // index '5'
					   0.8 * cos(radians(100.0)), 0.8 * sin(radians(100.0)), 0.0, 1.0, // index '6'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(100.0)), 0.8 * sin(radians(100.0)), 0.0, 1.0, // index '6
					   0.8 * cos(radians(110.0)), 0.8 * sin(radians(110.0)), 0.0, 1.0, // index '7'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(110.0)), 0.8 * sin(radians(110.0)), 0.0, 1.0, // index '7'
					   0.8 * cos(radians(120.0)), 0.8 * sin(radians(120.0)), 0.0, 1.0, // index '8'

					   0.0, 0.0, 1.0, 1.0,                                     //index '0'
					   0.8 * cos(radians(120.0)), 0.8 * sin(radians(120.0)), 0.0, 1.0, // index '8'
					   0.8 * cos(radians(130.0)), 0.8 * sin(radians(130.0)), 0.0, 1.0, // index '9'
};

GLfloat colors[] = { 1.0, 0.0, 0.0, 1.0,
					   1.0, 0.0, 0.0, 1.0,
					   1.0, 0.0, 0.0, 1.0,

					   0.0, 1.0, 0.0, 1.0,
					   0.0, 1.0, 0.0, 1.0,
					   0.0, 1.0, 0.0, 1.0,

					   0.0, 0.0, 1.0, 1.0,
					   0.0, 0.0, 1.0, 1.0,
					   0.0, 0.0, 1.0, 1.0,

					   1.0, 1.0, 0.0, 1.0,
					   1.0, 1.0, 0.0, 1.0,
					   1.0, 1.0, 0.0, 1.0,

					   0.0, 1.0, 1.0, 1.0,
					   0.0, 1.0, 1.0, 1.0,
					   0.0, 1.0, 1.0, 1.0,

					   1.0, 0.0, 1.0, 1.0,
					   1.0, 0.0, 1.0, 1.0,
					   1.0, 0.0, 1.0, 1.0,

					   1.0, 1.0, 0.0, 1.0,
					   1.0, 1.0, 0.0, 1.0,
					   1.0, 1.0, 0.0, 1.0,

					   1.0, 0.0, 0.0, 1.0,
					   1.0, 0.0, 0.0, 1.0,
					   1.0, 0.0, 0.0, 1.0,

};

GLushort indices[] = { 0, 1, 2, 
					   0, 2, 3, 
					   0, 3, 4, 
					   0, 4, 5, 
					   0, 5, 6, 
					   0, 6, 7, 
					   0, 7, 8, 
					   0, 8, 9,
};