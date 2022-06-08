#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Plane.h"
#include "Cube.h"
#include "ShadeBasic.h"

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

GLuint matrix_loc;
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint program;

//different boolean variables

bool show_line = false;


const double kPI = 3.1415926535897932384626433832795;

mat4 view_matrix(1.0f);
mat4 projection_matrix(1.0f);

mat4 model_matrix(1.0f);
mat4 shear_matrix = mat4(1, 0.0, 0, 0, 0.25, 1.0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

//Add light components
vec4 light_position(5.0, 10.0, 5.0, 1.0);
vec3 light_ambient(0.3, 0.3, 0.3);
vec3 light_color(1.0, 1.0, 1.0);
vec3 material_color(0.9, 0.5, 0.3);
float shininess = 50.0f;

// uniform indices of light
GLuint ambient_loc;
GLuint light_source_loc;
GLuint light_position_loc;
GLuint material_color_loc;

GLfloat eye[3] = { 0.0f, 2.5f, 8.0f };
GLfloat center[3] = { 0.0f, 0.0f, 0.0f };
GLfloat offset = 0.0;

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

	program = initShaders("shader_picnic_table.vs", "shader_picnic_table.fs");

	// attribute indices
	model_matrix = mat4(1.0f);
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	matrix_loc = glGetUniformLocation(program, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	
	createPlane();
	createCube();
	createPyramid();

	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	projection_matrix = perspective(radians(90.0f), 1.0f, 1.0f, 20.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Add uniform indices for light and material

	ambient_loc = glGetUniformLocation(program, "Ambient");
	glUniform3fv(ambient_loc, 1, (GLfloat*)&light_ambient[0]);
	
	material_color_loc = glGetUniformLocation(program, "MaterialColor");
	glUniform3fv(material_color_loc, 1, (GLfloat*)&material_color[0]);

	light_source_loc = glGetUniformLocation(program, "LightColor");
	glUniform3fv(light_source_loc, 1, (GLfloat*)&light_color[0]);

	light_position_loc = glGetUniformLocation(program, "LightPosition");
	glUniform4fv(light_position_loc, 1, (GLfloat*)&light_position[0]);
}


void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//convert light position to the camer space
	vec4 light_position_camera = view_matrix * light_position;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_position_camera[0]);

	// Setup view matrix
	view_matrix = glm::lookAt(glm::vec3(6.0 * cos(radians(rotateAngle)), eye[1], 6.0 * sin(radians(rotateAngle))), glm::vec3(center[0], center[1], center[2]), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	// Draws the pole  
	material_color = vec3(0.9, 0.5, 0.3);
	glUniform3fv(material_color_loc, 1, (GLfloat*)&material_color[0]);
	model_matrix = translate(mat4(1.0f), vec3(offset, 0.0, 0.0))*scale(mat4(1.0f), vec3(0.20, 8.0, 0.20));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	drawCube();

	// Draws the tabletop
	model_matrix = translate(mat4(1.0f), vec3(offset, 0.33, 0.0)) * scale(mat4(1.0f), vec3(3.0, 0.25, 3.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	// Draws the four legs
	model_matrix = shear_matrix * translate(mat4(1.0f), vec3(-0.2, -2.0, 0.0)) * scale(mat4(1.0f), vec3(0.5, 4.0, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0, 1.0, 0.0)) * shear_matrix * translate(mat4(1.0f), vec3(-0.2, -2.0, 0.0)) * scale(mat4(1.0f), vec3(0.5, 4.0, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = rotate(mat4(1.0f), radians(180.0f), vec3(0.0, 1.0, 0.0)) * shear_matrix * translate(mat4(1.0f), vec3(-0.2, -2.0, 0.0)) * scale(mat4(1.0f), vec3(0.5, 4.0, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = rotate(mat4(1.0f), radians(270.0f), vec3(0.0, 1.0, 0.0)) * shear_matrix * translate(mat4(1.0f), vec3(-0.2, -2.0, 0.0)) * scale(mat4(1.0f), vec3(0.5, 4.0, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	// Draws the four stools
	model_matrix = translate(mat4(1.0f), vec3(3.0, -3.0, 0.0)) * scale(mat4(1.0f), vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = translate(mat4(1.0f), vec3(0.0, -3.0, 3.0)) * scale(mat4(1.0f), vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = translate(mat4(1.0f), vec3(-3.0, -3.0, 0.0)) * scale(mat4(1.0f), vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = translate(mat4(1.0f), vec3(0.0, -3.0, -3.0)) * scale(mat4(1.0f), vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	// Draws the umbrella
	model_matrix = translate(mat4(1.0f), vec3(0.0, 2.5, 0.0)) * scale(mat4(1.0f), vec3(2.5, 0.5, 2.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawPyramid();

	// Draws the ground
	model_matrix = translate(mat4(1.0f), vec3(offset, -4.0, 0.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	material_color = vec3(0.0, 1.0, 0.0);
	glUniform3fv(material_color_loc, 1, (GLfloat*)&material_color[0]);
	drawPlane();
	
	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y){

	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}

void RotateCamera(int n)
{
	rotateAngle += 0.7;
	glutPostRedisplay();
	glutTimerFunc(10, RotateCamera, 1);
}

/*********/
int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(800, 800);

	glutCreateWindow("Picnic Table");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutTimerFunc(10, RotateCamera, 1);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	
	return 0;
}

/*************/



