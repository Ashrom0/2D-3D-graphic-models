#include <GL/glew.h>
#include <GL/freeglut.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GLM.h"


#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.1415926535897932384626433832795

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "Skybox.h"
#include "Cube.h"
#include "Sphere.h"

using namespace glm;

GLuint matrix_loc;

mat4 view(1.0f);
mat4 model(1.0f);
mat4 projection(1.0f);
mat4 model_view(1.0f);

GLuint program;
float aspect = 0.0;
GLuint textureID;
GLuint cubeTexID;
GLuint sphereTexID;

bool switch_models = false;
bool switch_mode = false;
bool reflect_switch = false;
bool reflect_switch_1 = false;

/* Class GLM object*/
GLMmodel* objmodel = NULL;

GLuint vao, vbo, ebo;

const double kPI = 3.1415926535897932384626433832795;
int gViewportWidth, gViewportHeight;

vec3 cameraPos;
float angle = 0.0;
float reflectFactor = 0.0;

void Initialize();
void Display(void);
void Reshape(int width, int height);
void setMatrices();
unsigned int loadCubeMapTexture(const char* posX, const char* negX, const char* posY, const char* negY, const char* posZ, const char* negZ);

/********************************************************************************/
const GLchar* ReadFile(const char* filename)
{
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

	GLchar* source = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;

	return (GLchar*)(source);
}

GLuint initShaders(const char *v_shader, const char *f_shader) {

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

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free((GLchar*)log);


	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetShaderInfoLog(f, len, &len, log);
		printf("Fragment Shader compilation failed: %s\n", log);
		free((GLchar*)log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);

	GLint linked;
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free((char*)log);
	}
	glUseProgram(p);
	return p;

}
/****************************************************************************/
unsigned int loadCubeMapTexture(const char* posX, const char* negX, const char* posY, const char* negY, const char* posZ, const char* negZ) {

	ILboolean success;
	unsigned int imageID;
	GLuint textureID;
	GLuint faceTarget[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	const char* files[6];

	files[0] = posX;
	files[1] = negX;
	files[2] = posY;
	files[3] = negY;
	files[4] = posZ;
	files[5] = negZ;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID); /* Texture name generation */
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// Load Textures for Cube Map

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	ilGenImages(1, &imageID);
	ilBindImage(imageID); /* Binding of DevIL image name */

	for (int i = 0; i < 6; ++i) {
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		success = ilLoadImage((ILstring)files[i]);

		if (!success) {
			printf("Couldn't load texture file.\n");
			// The operation was not sucessfull hence free image and texture 
			ilDeleteImages(1, &imageID);
		}

		/* Convert image to RGBA */
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		/* Create and load textures to OpenGL */
		glTexImage2D(faceTarget[i], 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

		printf("Texture Loaded: %s\n", files[i]);
	}
	printf("Cube Map Loaded Successfully.\n");


	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	/* Because we have already copied image data into texture data we can release memory used by image. */
	ilDeleteImages(1, &imageID);

	// add information to the log

	return textureID;
}
/****************************************************************************/

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

void Initialize(void){
	// Create the program for rendering the model

	createSkybox();
	createCube();
	
	if (!objmodel) {
		objmodel = glmReadOBJ("StatueTextured.obj");
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

	// Create and compile our GLSL program from the shaders
	program = initShaders("cubemap_reflect.vs", "cubemap_reflect.fs");
	
	// Use our shader
	glUseProgram(program);
	
	cameraPos = vec3(0.0f, 0.0f, 6.0f);

	createSphere();
	glClearColor(1.0, 1.0, 1.0, 1.0);

	textureID = loadCubeMapTexture("images/pos_x.tga", "images/neg_x.tga", "images/pos_y.tga", "images/neg_y.tga", "images/pos_z.tga", "images/neg_z.tga");
	glUniform1i(glGetUniformLocation(program, "CubeMapTex"), 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);


	//glUniform1i(glGetUniformLocation(program, "Tex1"), 1);
	sphereTexID = loadTexture("golden.jpg");
	glUniform1i(glGetUniformLocation(program, "Tex1"), 1);
	cubeTexID = loadTexture("tile.jpg");
	glUniform1i(glGetUniformLocation(program, "Tex2"), 2);

	vec4 material_color(1.0f, 0.5f, 0.5f, 1.0f);
	glUniform4fv(glGetUniformLocation(program, "MaterialColor"), 1, (GLfloat*)&material_color[0]);
	
	//glUniform1i(glGetUniformLocation(program, "CubeMapTex"), 0);
}
/************************************************************************************************/
void Display(void){
	
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sphereTexID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cubeTexID);
	glUniform1f(glGetUniformLocation(program, "ReflectFactor"), reflectFactor);

	if (switch_models) {
		if (switch_mode) {
			model = mat4(1.0f) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0));
			glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 1); //static
			glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 0);
			glUniform1i(glGetUniformLocation(program, "TextureModel"), 1);
			setMatrices();
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);

			glBindVertexArray(0);
		}
		else {
			if (reflect_switch) {
				model = mat4(1.0f) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0)); //mat color
				glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 0);
				glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 0);
				glUniform1i(glGetUniformLocation(program, "TextureModel"), 0);
				setMatrices();
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
			else {
				model = mat4(1.0f) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0)); //gold text
				glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 1);
				glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 0);
				glUniform1i(glGetUniformLocation(program, "TextureModel"), 1);
				setMatrices();
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);

				glBindVertexArray(0);
			}
		}
	}
	else {
		if (switch_mode) {
			model = mat4(1.0f) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0));
			glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 1); //static
			glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 0);
			glUniform1i(glGetUniformLocation(program, "TextureModel"), 1);
			setMatrices();
			drawSphere();
		}
		else {
			if (reflect_switch) {
				//glBindTexture(GL_TEXTURE_2D, cubeTexID);
				model = mat4(1.0f) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0));
				glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 0); //mat color
				glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 0);
				glUniform1i(glGetUniformLocation(program, "TextureModel"), 0);
				setMatrices();
				drawSphere();
			}
			else {
				//glBindTexture(GL_TEXTURE_2D, cubeTexID);
				model = mat4(1.0f) * scale(mat4(1.0f), vec3(3.0, 3.0, 3.0));
				glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 0); //gold text
				glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 0);
				glUniform1i(glGetUniformLocation(program, "TextureModel"), 1);
				setMatrices();
				drawSphere();
			}
		}
		
	}

    glUseProgram(program);
	view = glm::lookAt(vec3(6.0f * cos(radians(angle)), 0.0f, 6.0f * sin(radians(angle))), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(radians(70.0f), aspect, 0.3f, 100.0f);
	glUniform3fv(glGetUniformLocation(program, "WorldCameraPosition"), 1, (GLfloat*)&cameraPos[0]);

	glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 1);
	model = mat4(1.0f);
	
	setMatrices();
	drawSkybox();

	model = translate(mat4(1.0f), vec3(0.0, -4.5, 0.0)) * scale(mat4(1.0f), vec3(1.5, 1.5, 1.5));
	glUniform1i(glGetUniformLocation(program, "DrawSkyBox"), 0);
	glUniform1i(glGetUniformLocation(program, "DrawPedestal"), 1);
	glUniform1i(glGetUniformLocation(program, "TextureModel"), 0);
	setMatrices();
	drawCube();

	glutSwapBuffers();
}

/*******************************************************************************/
void setMatrices()
{
	mat4 mv = view * model;
	vec4 material_color(0.4, 0.1, 0.1, 1.0);
	mat4 mvp = projection*mv;
	glUniformMatrix4fv(glGetUniformLocation(program, "ModelMatrix"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "MVP"), 1, GL_FALSE, (GLfloat*)&mvp[0]);
	glUniform4fv(glGetUniformLocation(program, "MaterialColor"), 1, (GLfloat*)&material_color[0]); 
}

/*****************************************************************************/

void Reshape(int width, int height){
	
	
	glViewport(0, 0, width, height);
	
	aspect = float(width) / float(height);
}

/**************************************************************************************/
void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	
	case 'm':case 'M':
		switch_models = !switch_models;
		break;
	
	case 's':case 'S':
		switch_mode = !switch_mode;
		break;

	case 't':case 'T':
		reflect_switch = !reflect_switch;
		reflect_switch_1 = !reflect_switch_1;
		break;

	}
	glutPostRedisplay();
}

/******************************************************************************************/

void Delay(int n) {

	angle += 1.0f;
	if (reflect_switch_1)
		reflectFactor += 0.01f;
	else
		reflectFactor -= 0.01f;
	if (reflectFactor <= 0.0f || reflectFactor >= 1.0f)
		reflect_switch_1 = !reflect_switch_1;
	glutPostRedisplay();
	glutTimerFunc(50, Delay, n);


}
/************************************************************************************************************************************************************/
int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Environment mapping");

	if (glewInit()){
		printf("Unable to initialize GLEW ... exiting\n");
	}

	ilInit();
	Initialize();
	printf("%s\n",glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	
	glutReshapeFunc(Reshape);

	glutTimerFunc(50, Delay, 0);
	glutMainLoop();
	return 0;
}


