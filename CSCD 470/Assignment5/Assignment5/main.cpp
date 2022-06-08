#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GLM.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "Disk.h"

#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

using namespace glm;

GLuint matrix_loc;
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint program;
GLuint cubeTexID;
GLuint diskTexID;
GLuint texIDOne;
GLuint texIDTwo;

/* Class GLM object*/
GLMmodel* objmodel = NULL;

//different boolean variables

bool show_line = false;
bool top_view = false;

GLfloat angle = 0.0;
GLuint vao, vbo, ebo;

float timer = 0.0f;

const double kPI = 3.1415926535897932384626433832795;

mat4 view_matrix(1.0f);
mat4 projection_matrix(1.0f);
mat4 model_matrix(1.0f);

//Add light components
vec4 light_position(10.0f, 6.0f, 8.0f, 1.0f);
vec3 light_ambient(0.3, 0.3, 0.3);
vec3 light_color(0.9, 0.9, 0.9);
float material_shininess = 50.0f;

// uniform indices of light
GLuint ambient_loc;
GLuint light_source_loc;
GLuint light_position_loc;
GLuint material_color_loc;


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
void updateVertexNormals(vec3* vertices, vec3* norms) {

	vec3 p1, p2, p3, n;

	for (int i = 0; i < objmodel->numnormals; i++) {
		norms[i] = vec3(0.0, 0.0, 0.0);
	}

	for (int index = 0; index < objmodel->numindices; index += 3) {

		p1 = vertices[objmodel->indices[index + 0]];
		p2 = vertices[objmodel->indices[index + 1]];
		p3 = vertices[objmodel->indices[index + 2]];

		n = normalize(cross((p2 - p1), (p3 - p1)));

		norms[objmodel->indices[index + 0]] += n;
		norms[objmodel->indices[index + 1]] += n;
		norms[objmodel->indices[index + 2]] += n;
	}


	for (int i = 0; i < objmodel->numnormals; i++) {
		norms[i] = normalize(norms[i]);
	}

}

/***********************************************************************************/

void unitize(vec3 vertices[]) {
	float min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = max_x = vertices[0].x;
	min_y = max_y = vertices[0].y;
	min_z = max_z = vertices[0].z;

	//finding the min and max for xyz
	for (int i = 0; i < objmodel->numvertices; i++) {
		if (vertices[i].x < min_x)
			min_x = vertices[i].x;
		if (vertices[i].x > max_x)
			max_x = vertices[i].x;
		if (vertices[i].y < min_y)
			min_y = vertices[i].y;
		if (vertices[i].y > max_y)
			max_y = vertices[i].y;
		if (vertices[i].z < min_z)
			min_z = vertices[i].z;
		if (vertices[i].z > max_z)
			max_z = vertices[i].z;
	}

	//find xyz central points
	float center_x = (max_x + min_x) / 2;
	float center_y = (max_y + min_y) / 2;
	float center_z = (max_z + min_z) / 2;

	//calculate width/height and scale
	float width = abs(max_x - min_x);
	float height = abs(max_y - min_y);
	float depth = abs(max_z - min_z);
	float scale = max(depth, max(width, height));

	for (int i = 0; i < objmodel->numvertices; i++) {
		//center the model
		vertices[i].x -= center_x;
		vertices[i].y -= center_y;
		vertices[i].z -= center_z;

		//scale the model to fit inside a 2x2x2 box
		vertices[i].x /= scale / 2;
		vertices[i].y /= scale / 2;
		vertices[i].z /= scale / 2;
	}
}


void Timer(int n){
	timer += 0.1f;
	glutPostRedisplay();
	glutTimerFunc(100, Timer, n);
}

/*******************************************************/
void Initialize(void){
	// Create the program for rendering the model
	if (!objmodel) {
		objmodel = glmReadOBJ("PumpkinTextured.obj");
		if (!objmodel) {
			printf("OBJ file does not exist \n");
			exit(0);
		}
		unitize(objmodel->vertices);
		updateVertexNormals(objmodel->vertices, objmodel->normals);
	}

	// Fill up the buffers!
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint offset = 0;
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * (objmodel->numvertices) + sizeof(vec3) * (objmodel->numvertices) + sizeof(vec2) * (objmodel->numvertices), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * (objmodel->numvertices), objmodel->vertices);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	offset += sizeof(vec3) * (objmodel->numvertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * (objmodel->numvertices), objmodel->normals);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(vec3) * (objmodel->numvertices)));
	glEnableVertexAttribArray(1);  // Vertex normal

	offset += sizeof(vec3) * (objmodel->numvertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * (objmodel->numvertices), objmodel->textures);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(vec3) * (objmodel->numvertices + objmodel->numvertices)));
	glEnableVertexAttribArray(2);  // Vertex textures

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (objmodel->numindices), objmodel->indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	program = initShaders("shader_texture.vs", "shader_texture.fs");

	createDisk();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// uniform indices of light

	light_position_loc = glGetUniformLocation(program, "LightPosition");
	glUniform4fv(light_position_loc, 1, (GLfloat*)&light_position[0]);

	ambient_loc = glGetUniformLocation(program, "Ambient");
	glUniform3fv(ambient_loc, 1, (GLfloat*)&light_ambient[0]);

	light_source_loc = glGetUniformLocation(program, "LightColor");
	glUniform3fv(light_source_loc, 1, (GLfloat*)&light_color[0]);

	glUniform1i(glGetUniformLocation(program, "Tex1"), 0);
	cubeTexID = loadTexture("test.dds");
	glActiveTexture(GL_TEXTURE0);

	glUniform1i(glGetUniformLocation(program, "Tex2"), 1);
	texIDOne = loadTexture("test3.dds");
	glActiveTexture(GL_TEXTURE1);

	glUniform1i(glGetUniformLocation(program, "Tex3"), 2);
	texIDTwo = loadTexture("interior.jpg");
	glActiveTexture(GL_TEXTURE2);

	glUniform1i(glGetUniformLocation(program, "Tex4"), 3);
	diskTexID = loadTexture("marble.jpg");
	glActiveTexture(GL_TEXTURE3);
}


void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	view_matrix = glm::lookAt(vec3(0.0, 0.5, 4.0), vec3(0.0, 0.0, 0.0), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	// attribute indices
	model_matrix = rotate(mat4(1.0f), radians(angle), vec3(0.0,1.0,0.0));
	matrix_loc = glGetUniformLocation(program, "model_matrix");
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);

	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	projection_matrix = perspective(radians(60.0f), 1.0f, 1.0f, 20.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	vec4 lightPosEye = view_matrix*light_position;
	glUniform4fv(light_position_loc, 1, (GLfloat*)&lightPosEye[0]);

	glUniform1f(glGetUniformLocation(program, "Timer"), timer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texIDOne);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texIDTwo);
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

	// Disk texture
	glUniform1f(glGetUniformLocation(program, "On"), 1);
	glBindTexture(GL_TEXTURE_2D, diskTexID);
	model_matrix = translate(mat4(1.0f), vec3(0.0, -1.0, 0.0)) * scale(mat4(1.0f), vec3(2.0, 2.0, 2.0));
	glUniformMatrix4fv(matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawDisk();


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
	angle += 1.0f;
	glutPostRedisplay();
	glutTimerFunc(100, Rotate, 1);
}

/*********/
int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(500, 500);

	glutCreateWindow("Textured Pumpkin");

	if (glewInit()){
		printf("%s\n", "Unable to initialize GLEW ... exiting.");
	}
	ilInit();
	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(100, Rotate, 1);
	glutTimerFunc(100, Timer, 1);
	glutMainLoop();
	
	return 0;
}

/*************/



