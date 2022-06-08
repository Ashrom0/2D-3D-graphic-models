#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Plane.h"
#include "Cube.h"
#include "Parasol.h"

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

GLuint matrix_loc;
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint program;

//different boolean variables

bool show_line = false;
bool open_parasol = true;

const double kPI = 3.1415926535897932384626433832795;

mat4 view_matrix(1.0f);
mat4 projection_matrix(1.0f);
mat4 model_matrix(1.0f);


GLfloat eye[3] = { 0.0f, 4.5f, 6.0f };
GLfloat center[3] = { 0.0f, 0.0f, 0.0f };
GLfloat offset = 0.0;

GLfloat aspect = 0.0;
GLfloat rotateAngle = 0.0f;

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

/*******************************************************/
void Initialize(void){
	// Create the program for rendering the model

	program = initShaders("shader.vs", "shader.fs");

	
	// attribute indices
	model_matrix = mat4(1.0f);
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	matrix_loc = glGetUniformLocation(program, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	
	createPlane();
	createCube();
	createParasol();


	projection_matrix = perspective(radians(90.0f), aspect, 1.0f, 20.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (show_line)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//adding aprasol, open and close the parasol
	if(open_parasol)
		model_matrix = translate(mat4(1.0f), vec3(0, 2.0, 0)) * rotate(mat4(1.0f), radians(rotateAngle), vec3(0.0, 1.0, 0.0));
	else
		model_matrix = translate(mat4(1.0f), vec3(0, 0.35, 0)) * scale(mat4(1.0f), vec3(0.5, 4.25, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawParasol();
	
	// Setup view matrix 

	view_matrix = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(center[0], center[1], center[2]), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	//Add the aspect ratio! You also need to use glutReshapeFunc!

	projection_matrix = perspective(radians(90.0f), aspect, 1.0f, 20.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	//Cube is being drawn without any transformation

	model_matrix = scale(mat4(1.0f), vec3(0.25f, 5.0f, 0.25f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	drawCube();
	
	//Plane is at the bottom of the cube and is being drawn with some transformation
	model_matrix = translate(mat4(1.0f), vec3(0.0, -2.5, 0.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	drawPlane();

	//making the first chair!
	//1st leg
	mat4 shear_matrix = { 1.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
	model_matrix = translate(mat4(1.0f), vec3(-1.5, -2.0, -1.0)) * shear_matrix * scale(mat4(1.0f), vec3(0.25, 1.0, 0.25));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//2nd leg
	mat4 rotate_matrix = rotate(mat4(1.0f), radians(180.0f), vec3(0.0, 1.0, 0.0));
	model_matrix = translate(mat4(1.0f), vec3(-1.0, -2.0, -1.0)) * rotate_matrix * shear_matrix * scale(mat4(1.0f), vec3(0.25, 1.0, 0.25));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	
	//3rd leg
	model_matrix = translate(mat4(1.0f), vec3(-1.25, -2.0, -0.75)) * rotate(mat4(1.0f),
		radians(90.0f), vec3(0.0, 1.0, 0.0)) * shear_matrix * scale(mat4(1.0f),
			vec3(0.25, 1.0, 0.25));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//4th leg
	model_matrix = translate(mat4(1.0f), vec3(-1.25, -2.0, -1.25)) * rotate(mat4(1.0f),
		radians(90.0f), vec3(0.0, 1.0, 0.0)) * rotate_matrix * shear_matrix * scale(mat4(1.0f),
			vec3(0.25, 1.0, 0.25));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//stool part of the chair
	model_matrix = translate(mat4(1.0f), vec3(-1.25, -1.25, -1.0)) * scale(mat4(1.0f), vec3(0.25f, 0.55f, 0.25f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//seat of the chair
	model_matrix = translate(mat4(1.0f), vec3(-1.25, -1.0, -1.0)) * scale(mat4(1.0f), vec3(1.5f, 0.25f, 1.5f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//back support of chair, ending of first chair
	model_matrix = translate(mat4(1.0f), vec3(-1.25, -0.25, -1.75)) * scale(mat4(1.0f), vec3(1.5f, 1.5f, 0.0f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();


	//Time for the second chair to be assembled, IKEA style!!!
	//1st leg
	model_matrix = translate(mat4(1.0f), vec3(1.0, -2.0, 0.75)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0)) 
		* shear_matrix * scale(mat4(1.0f), vec3(0.15, 0.5, 0.15));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//2nd leg
	model_matrix = translate(mat4(1.0f), vec3(2.0, -2.0, 0.75)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0))
		* shear_matrix * scale(mat4(1.0f), vec3(0.15, 0.5, 0.15));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//3rd leg
	model_matrix = translate(mat4(1.0f), vec3(2.0, -2.0, -1.25)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0)) 
		* rotate_matrix * shear_matrix * scale(mat4(1.0f), vec3(0.15, 0.5, 0.15));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//4th leg
	model_matrix = translate(mat4(1.0f), vec3(1.0, -2.0, -1.25)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0)) 
		* rotate_matrix * shear_matrix * scale(mat4(1.0f), vec3(0.15, 0.5, 0.15));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//A series of beams and flaps to make a beach chair style
	//beams
	model_matrix = translate(mat4(1.0f), vec3(1.0, -1.75, -0.25)) * scale(mat4(1.0f), vec3(0.15f, 0.15f, 1.95f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(2.0, -1.75, -0.25)) * scale(mat4(1.0f), vec3(0.15f, 0.15f, 1.95f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(1.0, -1.0, -1.5)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0)) 
		* shear_matrix * scale(mat4(1.0f), vec3(0.15, 1.5, 0.15));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(2.0, -1.0, -1.5)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0)) 
		* shear_matrix * scale(mat4(1.0f), vec3(0.15, 1.5, 0.15));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	//flapss
	model_matrix = translate(mat4(1.0f), vec3(1.5, -1.65, 0.45)) * scale(mat4(1.0f), vec3(1.0f, 0.0f, 0.25f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(1.5, -1.65, 0.0)) * scale(mat4(1.0f), vec3(1.0f, 0.0f, 0.25f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(1.5, -1.65, -0.45)) * scale(mat4(1.0f), vec3(1.0f, 0.0f, 0.25f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(1.5, -1.65, -0.9)) * scale(mat4(1.0f), vec3(1.0f, 0.0f, 0.25f));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = translate(mat4(1.0f), vec3(1.5, -1.45, -1.25)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0))
		* shear_matrix * scale(mat4(1.0f), vec3(0.0, 0.25, 1.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(1.5, -0.95, -1.5)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0))
		* shear_matrix * scale(mat4(1.0f), vec3(0.0, 0.25, 1.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();
	model_matrix = translate(mat4(1.0f), vec3(1.5, -0.5, -1.75)) * rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0))
		* shear_matrix * scale(mat4(1.0f), vec3(0.0, 0.25, 1.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	glutSwapBuffers();
}

//reuse this in glutTimerFunc!

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		eye[2] += 0.25;
	if (key == GLUT_KEY_DOWN)
		eye[2] -= 0.25;
}

void Reshape(int width, int height)
{

	glViewport(0, 0, width, height);
	aspect = float(width) / float(height);
}

void rotate(int n) {
	switch (n) {
	case 1:
		rotateAngle += 5.0f;
		
		glutPostRedisplay();
		glutTimerFunc(100, rotate, 1);
		break;
		
	}

}


void keyboard(unsigned char key, int x, int y){

	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 's':case 'S':
		show_line = !show_line;
		break;
	//Add othere statements here!
	
	case 'r':case 'R':
		eye[2] = 6.0f;
		break;

	case 'o':case 'O':
		open_parasol = !open_parasol;
		break;

	} 
	glutPostRedisplay();
}


/*********/
int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(800, 800);

	glutCreateWindow("Parasol");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);

	//Add other callback functions here!
	glutTimerFunc(100, rotate, 1);
	glutSpecialFunc(SpecialKeys);
	glutReshapeFunc(Reshape);
	glutMainLoop();
	
	return 0;
}

/*************/