#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265f
#endif


#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.141592653589793238//4626433832795

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


using namespace glm;


/* GLMtriangle: Structure that defines a triangle in a model.
 */


 /* GLMmodel: Structure that defines a model.
  */
typedef struct _GLMmodel {

	GLuint   numvertices;         /* number of vertices in model */
	vec3* vertices;         /* array of vertices  */


	GLuint   numnormals;          /* number of normals in model */
	vec3* normals;        /* array of normals */

	GLuint   numtextures;          /* number of textures in model */
	vec2* textures;        /* array of tex coordinates */

	GLuint       numtriangles;    /* number of triangles in model */


	GLuint   numindices;
	GLuint* indices;


} GLMmodel;





GLvoid
glmDelete(GLMmodel* model);

/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * glmDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.
 */
GLMmodel*
glmReadOBJ(const char* filename);

