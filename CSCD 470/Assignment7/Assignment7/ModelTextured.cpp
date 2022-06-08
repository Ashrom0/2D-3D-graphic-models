#include <GL/glew.h>
#include <math.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <GL/freeglut.h>


#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.1415926535897932384626433832795

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


//#include "Plane.h"
#include "Sphere.h"

using namespace glm;


GLuint matrix_loc;
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint program;
GLuint texID, normalTexID;
GLuint tex2ID, normalTex2ID;



float angle = 0.0;



//different boolean variables

bool show_line = false;
bool normal_mapped = true;
bool surface_change = false;


const double kPI = 3.1415926535897932384626433832795;

mat4 view_matrix(1.0f);
mat4 projection_matrix(1.0f);
mat4 model_matrix(1.0f);

//Add light components
vec4 light_position(20.0, 20.0, 20.0, 1.0);
vec3 light_ambient(0.3, 0.3, 0.3);
vec3 light_color(1.0, 1.0, 1.0);

// uniform indices of light
GLuint ambient_loc;
GLuint light_source_loc;
GLuint light_position_loc;
GLuint material_color_loc;

GLfloat eye[3] = { 0.0f, 0.0f, 22.0f };
GLfloat center[3] = { 0.0f, 0.0f, 0.0f };


char* ReadFile(const char* filename);
GLuint initShaders(const char* v_shader, const char* f_shader);
void keyboard(unsigned char key, int x, int y);



char* ReadFile(const char* filename) {

	FILE* infile;
#ifdef WIN32
	fopen_s(&infile, filename, "rb");
#else
	infile = fopen(filename, "rb");
#endif


	if (!infile) {
		printf("Unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	char* source = (char*)malloc(len + 1);
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;
	return (source);

}

/*************************************************************/
/*************************************************************/

GLuint initShaders(const char* v_shader, const char* f_shader) {

	GLuint p = glCreateProgram();

	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vs = ReadFile(v_shader);
	const char * fs = ReadFile(f_shader);

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);

	free((char*)vs);
	free((char*)fs);

	glCompileShader(v);

	GLint compiled;

	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &len);

		char* log = (char*)malloc(len + 1);

		glGetShaderInfoLog(v, len, &len, log);

		printf("Vertex Shader compilation failed: %s\n", log);

		free(log);

	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {

		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)malloc(len + 1);
		glGetShaderInfoLog(f, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);
	GLint linked;

	glGetProgramiv(p, GL_LINK_STATUS, &linked);

	if (!linked) {

		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)malloc(len + 1);
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}

	glUseProgram(p);

	return p;

}
/********************************************************/
/************************************/
unsigned int loadTexture(const char* filename) {

	ILboolean success;
	unsigned int imageID;
	ilGenImages(1, &imageID);

	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	success = ilLoadImage((ILstring)filename);

	if (!success) {
		printf("Couldn't load the following texture file: %s", filename);
		// The operation was not sucessfull hence free image and texture 
		ilDeleteImages(1, &imageID);
		return 0;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Because we have already copied image data into texture data
	we can release memory used by image. */

	ilDeleteImages(1, &imageID);
	return tid;
}

/************************************************************/
void Initialize(void){

	
	// Create the program for rendering the model

	program = initShaders("normal_texture.vs", "normal_texture.fs");


	
	//createPlane();
	createSphere();
	


	
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	view_matrix = glm::lookAt(vec3(eye[0], eye[1], eye[2]), glm::vec3(center[0], center[1], center[2]), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	// attribute indices
	model_matrix = mat4(1.0f);
	matrix_loc = glGetUniformLocation(program, "model_matrix");
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	projection_matrix = perspective(radians(60.0f), 1.0f, 1.0f, 40.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// uniform indices of light

	light_position_loc = glGetUniformLocation(program, "LightPosition");
	glUniform4fv(light_position_loc, 1, (GLfloat*)&light_position[0]);

	ambient_loc = glGetUniformLocation(program, "Ambient");
	glUniform3fv(ambient_loc, 1, (GLfloat*)&light_ambient[0]);

	light_source_loc = glGetUniformLocation(program, "LightColor");
	glUniform3fv(light_source_loc, 1, (GLfloat*)&light_color[0]);

	

	glActiveTexture(GL_TEXTURE0);

	texID = loadTexture("earth.jpg");
	
	glUniform1i(glGetUniformLocation(program, "EarthTex"), 0);

	glActiveTexture(GL_TEXTURE1);

	normalTexID = loadTexture("earthNormal.png");
	glUniform1i(glGetUniformLocation(program, "NormalTex"), 1);

	glActiveTexture(GL_TEXTURE2);

	tex2ID = loadTexture("mars.jpg");

	glUniform1i(glGetUniformLocation(program, "MarsTex"), 2);

	glActiveTexture(GL_TEXTURE3);

	normalTex2ID = loadTexture("marsNormal.png");
	glUniform1i(glGetUniformLocation(program, "NormalTex2"), 3);


	
	glClearColor(1.0, 1.0, 1.0, 1.0);

	//glClearColor(0.0, 0.0, 0.0, 1.0);


}

/******************************************************************/

void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (show_line) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*if (normal_mapped)
		glUniform1i(glGetUniformLocation(program, "NormalMapped"), 1);
	else
		glUniform1i(glGetUniformLocation(program, "NormalMapped"), 0);
	*/


	if (surface_change) {
		glUniform1i(glGetUniformLocation(program, "NormalMapped"), 0); //change to mars.jpg
		glUniform1i(glGetUniformLocation(program, "TexChange"), 1);
		if (normal_mapped) {
			glUniform1i(glGetUniformLocation(program, "NormalMapped"), 1); //change to marsNormal.png
			glUniform1i(glGetUniformLocation(program, "MapChange"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(program, "NormalMapped"), 0); //change to mars.jpg
			glUniform1i(glGetUniformLocation(program, "TexChange"), 1);
		}
	}
	else {
		glUniform1i(glGetUniformLocation(program, "NormalMapped"), 0); //change to earth.jpg
		glUniform1i(glGetUniformLocation(program, "TexChange"), 0);
		if (normal_mapped) {
			glUniform1i(glGetUniformLocation(program, "NormalMapped"), 1); //change to marsNormal.png
			glUniform1i(glGetUniformLocation(program, "MapChange"), 0);
		}
		else {
			glUniform1i(glGetUniformLocation(program, "NormalMapped"), 0); //change to mars.jpg
			glUniform1i(glGetUniformLocation(program, "TexChange"), 0);
		}
	}

	vec4 lightPosEye = view_matrix*light_position;
	glUniform4fv(light_position_loc, 1, (GLfloat*)&lightPosEye[0]);

	//Bind to the appropriate texture id

	model_matrix = rotate(mat4(1.0f), radians(angle), vec3(0.0, 1.0, 0.0)) * scale(mat4(1.0f), vec3(8.0, 8.0, 8.0));


	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTexID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2ID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalTex2ID);


	drawSphere();

	glutSwapBuffers();
}

/*********************************************************/
void keyboard(unsigned char key, int x, int y){

	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 's':case 'S':
		show_line = !show_line;
		break;

	case 'n':case 'N':
		normal_mapped = !normal_mapped;
		break;

	case 'm':case 'M':
		surface_change = !surface_change;
		break;
	}

	glutPostRedisplay();
}

void Delay(int n) {

	angle += 1.0f;
	glutPostRedisplay();
	glutTimerFunc(50, Delay, n);

}

/**********************************************************/

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(500, 500);

	glutCreateWindow("Normal Mapped Model");

	if (glewInit()){
		printf("%s\n", "Unable to initialize GLEW ... exiting.");
	}
	ilInit();
	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(50, Delay, 0);
	glutMainLoop();
	
	return 0;
}

/*************/



