#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
GLuint view_matrix_loc;


GLuint projection_matrix_loc;
GLboolean wire_frame = false;
glm::mat4 projection_matrix;
GLuint program;
GLboolean change_projection = false;


glm::mat4 view;
mat4 model_matrix;
GLfloat angle = 0.0;

vec4 light_position(0.0, 0.0, 9.8, 1.0);  // positional light source
vec4 light_position_camera(0.0, 0.0, 0.0, 1.0);
vec3 light_ambient(0.3, 0.3, 0.3);
vec3 light_color(1.0, 1.0, 1.0);

/********************************************/



char* ReadFile(const char* filename);
GLuint initShaders(const char* v_shader, const char* f_shader);
void Initialize();
void Display(void);

char* ReadFile(const char* filename) {
	FILE* infile;
#ifdef WIN32
	fopen_s(&infile, filename, "rb");
#else
	infile = fopen(filename, "r");
#endif
	if (!infile) {
		printf("Unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc(len + 1);
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;

	return (GLchar*)(source);
}

GLuint initShaders(const char* v_shader, const char* f_shader) {

	GLuint p = glCreateProgram();
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vs = ReadFile(v_shader);
	const char* fs = ReadFile(f_shader);

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

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetShaderInfoLog(f, len, &len, log);
		printf("Fragment Shader compilation failed: %s\n", log);
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

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}
	glUseProgram(p);
	return p;
}

/****************************************************/

/****************************************************/

void Initialize(void) {

	// Create the program for rendering the model
	program = initShaders("shader.vs", "shader.fs");

	// uniform indices
	matrix_loc = glGetUniformLocation(program, "model_matrix");

	// uniform indices

	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");

	createCube();
	createCone();
	createDisk();
	createCylinder();
	createSphere();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
/***********************************************************/

void Display(void) {

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLineWidth(5.0);

	//wires
	if (wire_frame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	projection_matrix = perspective(radians(45.0f), 1.0f, 2.0f, 20.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	//Change the camera position; 
	view = glm::lookAt(vec3(1.0f, 2.0f, 8.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view[0]);

	light_position = vec4(10.0f * cos(radians(angle)), 5.0f, 10.0f * sin(radians(angle)), 1.0f);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_position[0]);

	model_matrix = translate(mat4(1.0f), vec3(2.0, 0.0, -0.5)) * scale(mat4(1.0f), vec3(4.0, 4.0, 4.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = translate(mat4(1.0f), vec3(-0.5, 0.0, 1.5)) * scale(mat4(1.0f), vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCube();

	model_matrix = translate(mat4(1.0f), vec3(2.0, 0.5, -0.5)) * scale(mat4(1.0f), vec3(0.5, 1.0, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCone();

	model_matrix = translate(mat4(1.0f), vec3(1.0, -0.5, 1.0)) * scale(mat4(1.0f), vec3(0.5, 1.0, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCone();

	model_matrix = translate(mat4(1.0f), vec3(-1.0, 0.0, -1.5)) * scale(mat4(1.0f), vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawCylinder();

	model_matrix = translate(mat4(1.0f), vec3(-1.5, 0.0, 2.0)) * scale(mat4(1.0f), vec3(0.5, 0.5, 0.5));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawSphere();

	model_matrix = translate(mat4(1.0f), vec3(0.0, -0.5, 0.0)) * scale(mat4(1.0f), vec3(3.0, 0.0, 3.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawDisk();

	glFlush();

}

/**************************************************************************************/
void keyboard(unsigned char key, int x, int y) {

	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w':case 'W':
		wire_frame = !wire_frame;
		break;
	}
	glutPostRedisplay();
}
/*********************************************************************************************/
void timer(int n) {

	angle += 5.0f;
	glutPostRedisplay();
	glutTimerFunc(100, timer, 1);

}
/**********************************************************/

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Toon Shaded Pyramid");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(100, timer, 1);
	glutMainLoop();
	return 0;
}

/*************/
