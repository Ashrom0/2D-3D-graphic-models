#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "Cube.h"
#include "Cone.h"
#include "Disk.h"
#include "Cylinder.h"
#include "Sphere.h"

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;


GLuint matrix_loc;
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint program;
GLuint cubeTexID;
GLuint coneTexID;
GLuint diskTexID;
GLuint cylinderTexID;
GLuint sphereTexID;


//different boolean variables

bool show_line = false;
bool top_view = false;

GLfloat angle = 0.0;

const double kPI = 3.1415926535897932384626433832795;

mat4 view_matrix(1.0f);
mat4 projection_matrix(1.0f);
mat4 model_matrix(1.0f);

//Add light components
vec4 light_position(10.0f, 15.0f, 10.0f, 1.0f);
vec3 light_ambient(0.3, 0.3, 0.3);
vec3 light_color(0.9, 0.9, 0.9);
float material_shininess = 50.0f;

// uniform indices of light
GLuint ambient_loc;
GLuint light_source_loc;
GLuint light_position_loc;
GLuint material_color_loc;

//GLfloat eye[3] = { 6.0f, 3.0f, 8.0f };
//GLfloat center[3] = { 0.0f, 0.0f, 0.0f };


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

/*******************************************************/
void Initialize(void){
	// Create the program for rendering the model

	program = initShaders("shader_texture.vs", "shader_texture.fs");
	
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	view_matrix = glm::lookAt(vec3(1.0f, 2.0f, 8.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	// attribute indices
	model_matrix = mat4(1.0f);
	matrix_loc = glGetUniformLocation(program, "model_matrix");
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	projection_matrix = perspective(radians(45.0f), 1.0f, 1.0f, 50.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	createCube();
	createCone();
	createDisk();
	createCylinder();
	createSphere();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// uniform indices of light

	light_position_loc = glGetUniformLocation(program, "LightPosition");
	glUniform4fv(light_position_loc, 1, (GLfloat*)&light_position[0]);

	ambient_loc = glGetUniformLocation(program, "Ambient");
	glUniform3fv(ambient_loc, 1, (GLfloat*)&light_ambient[0]);

	light_source_loc = glGetUniformLocation(program, "LightColor");
	glUniform3fv(light_source_loc, 1, (GLfloat*)&light_color[0]);

	glUniform1i(glGetUniformLocation(program, "Tex1"), 0);
	//planeTexID = loadTexture("brick.jpg");
	cubeTexID = loadTexture("die.jpg");
	coneTexID = loadTexture("cone.jpg");
	diskTexID = loadTexture("vinyl.jpg");
	cylinderTexID = loadTexture("soda.jpg");
	sphereTexID = loadTexture("baseball2.jpg");
	glActiveTexture(GL_TEXTURE0);
}


void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	vec4 lightPosEye = view_matrix*light_position;
	glUniform4fv(light_position_loc, 1, (GLfloat*)&lightPosEye[0]);

	if (top_view) {
		view_matrix = glm::lookAt(vec3(0.0f, 15.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(-cos(radians(angle)), 1.0f, -sin(radians(angle))));
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	}
	else{
		view_matrix = glm::lookAt(vec3(15 * sin(radians(angle)), 1.0f, 15 * cos(radians(angle))), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	}

	//Bind to the appropriate texture id
	glBindTexture(GL_TEXTURE_2D, cubeTexID);
	model_matrix = translate(mat4(1.0f), vec3(1.0, 0.0, 0.0)) * scale(mat4(1.0f), vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	// Cone texture
	glBindTexture(GL_TEXTURE_2D, coneTexID);
	model_matrix = translate(mat4(1.0f), vec3(3.5, -1.0, -2.0)) * scale(mat4(1.0f), vec3(1.25, 1.75, 1.25));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCone();

	// Disk texture
	glBindTexture(GL_TEXTURE_2D, diskTexID);
	model_matrix = translate(mat4(1.0f), vec3(1.0, -1.0, 0.0)) * scale(mat4(1.0f), vec3(5.0, 5.0, 5.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawDisk();

	// Cylinder texture
	glBindTexture(GL_TEXTURE_2D, cylinderTexID);
	model_matrix = translate(mat4(1.0f), vec3(2.0, 0.5, 3.0)) * scale(mat4(1.0f), vec3(1.0, 1.5, 1.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCylinder();

	// Sphere texture
	glBindTexture(GL_TEXTURE_2D, sphereTexID);
	model_matrix = translate(mat4(1.0f), vec3(-2.0, 0.0, 2.0)) * scale(mat4(1.0f), vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawSphere();

	glutSwapBuffers();
}

/*********************************************************/
void keyboard(unsigned char key, int x, int y){

	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 't':case 'T':
		top_view = !top_view;
		break;
	}
	glutPostRedisplay();
}

/**********************************************************/

void Rotate(int n)
{
	angle += 5.0;
	glutPostRedisplay();
	glutTimerFunc(50, Rotate, 1);
}

/*********/
int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(500, 500);

	glutCreateWindow("Dice");

	if (glewInit()){
		printf("%s\n", "Unable to initialize GLEW ... exiting.");
	}
	ilInit();
	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(50, Rotate, 1);
	glutMainLoop();
	
	return 0;
}

/*************/



