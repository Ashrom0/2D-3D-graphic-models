#include <GL/glew.h> 
#include <math.h>
#include "GLM.h"

mat4 view(1.0f);
mat4 model(1.0f);
mat4 projection(1.0f);

GLuint program;
float aspect = 0.0;
GLfloat angle = 0.0;
GLuint vao;
unsigned int vbo[1];
GLuint ebo;

/* Class GLM object*/
GLMmodel* objmodel = NULL;
vec3* modifiedVerts = NULL;
vec3* modifiedNorms = NULL;

// vectors that the program came with

bool show_line = false;

vec4 light_position(10.0, 6.0, 8.0, 0.0);  // directional light source
GLfloat timeUpdate = 0.01;
GLfloat vscale = 0.0;

vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;

vec4 material_ambient(0.6, 0.5, 0.3, 1.0f);
vec4 material_diffuse(0.0, 1.0, 0.0, 1.0f);
vec4 material_specular(0.8, 0.8, 0.8, 1.0f);

vec4 lightintensity = vec4(0.9f, 0.9f, 0.9f, 1.0f);

GLboolean update = false;
GLboolean changeWave = false;

void Initialize();
void Display(void);
void Reshape(int width, int height);

GLchar* ReadFile(const char* filename);
GLuint initShaders(const char* v_shader, const char* f_shader);
/*************************************************************************************/
GLchar* ReadFile(const char* filename) {
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

/*******************************/
void updateVertexNormals(vec3* vertices, vec3* norms) {
	vec3 p1, p2, p3, n;
	for (int i = 0; i < objmodel->numnormals; i++) {
		norms[i] = vec3(0.0, 0.0, 0.0);
	}

	for (int index = 0; index < objmodel->numindices; index += 3) {

		p1 = vertices[objmodel->indices[index]];
		p2 = vertices[objmodel->indices[index + 1]];
		p3 = vertices[objmodel->indices[index + 2]];

		n = normalize(cross((p2 - p1), (p3 - p1)));

		norms[objmodel->indices[index]] += n;
		norms[objmodel->indices[index + 1]] += n;
		norms[objmodel->indices[index + 2]] += n;
	}

	for (int i = 0; i < objmodel->numnormals; i++) {

		norms[i] = normalize(objmodel->normals[i]);
	}

}
/***********************************************************************************/
void updateVertexCosine(vec3* ptr, vec3* verts, vec3* norms, int count) {
	vscale = 0.08 * cos(TWOPI * timeUpdate) + 0.06;

	for (int i = 0; i < count; i++)
	{
		ptr[i] = verts[i] + vscale * norms[i];
	}
}


/********************************************************************************/

void Initialize(void) {
	// Create the program for rendering the model
	if (!objmodel) {
		objmodel = glmReadOBJ("bunny.obj");
		if (!objmodel) {
			printf("OBJ file does not exist \n");
			exit(0);
		}
		updateVertexNormals(objmodel->vertices, objmodel->normals);
	}

	// Create and compile our GLSL program from the shaders

	program = initShaders("shader.vs", "shader.fs");
	glUseProgram(program);
	view = glm::lookAt(vec3(0.0f, 1.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	// Fill up the buffers!
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint offset = 0;
	glGenBuffers(1, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * (objmodel->numvertices) + sizeof(vec3) * (objmodel->numvertices), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * (objmodel->numvertices), objmodel->vertices);
	offset += sizeof(vec3) * (objmodel->numvertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * (objmodel->numvertices), objmodel->normals);

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(vec3) * (objmodel->numvertices)));
	glEnableVertexAttribArray(1);  // Vertex normal

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (objmodel->numindices), objmodel->indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	float material_shininess = 50.0;
	ambient_product = lightintensity * material_ambient;
	diffuse_product = lightintensity * material_diffuse;
	specular_product = lightintensity * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	modifiedVerts = (vec3*)malloc(sizeof(vec3) * (objmodel->numvertices));
	modifiedNorms = (vec3*)malloc(sizeof(vec3) * (objmodel->numvertices));

	glClearColor(0.8, 0.8, 0.8, 1.0);
}
/*******************************************************************************************/
void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	model = mat4(1.0f);
	projection = glm::perspective(radians(45.0f), aspect, 0.3f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	if (show_line)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	updateVertexCosine(modifiedVerts, objmodel->vertices, objmodel->normals, objmodel->numvertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * objmodel->numvertices, &modifiedVerts[0]);

	updateVertexNormals(modifiedVerts, modifiedNorms);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * objmodel->numvertices, sizeof(glm::vec3) * objmodel->numvertices, &modifiedNorms[0]);
	glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);

	glutSwapBuffers();
}

/******************************************************************************************/
void Reshape(int width, int height) {
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	glViewport(0, 0, width, height);

	aspect = float(width) / float(height);
}

/**************************************************************************************/

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'u':case 'U':
		show_line = !show_line;
		break;
	}
	glutPostRedisplay();
}
/**************************************************************************************/
void Delay(int n) {
	timeUpdate += .01;
	if (vscale < 0.001) {
		vec4 material_diffuse(1.0, 0.0, 0.0, 1.0);//red
		diffuse_product = lightintensity * material_diffuse;
		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	}

	if (vscale > 0.001) {
		vec4 material_diffuse(0.0, 1.0, 0.0, 1.0);//green
		diffuse_product = lightintensity * material_diffuse;
		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	}
	glutPostRedisplay();
	glutTimerFunc(50, Delay, n);

}


/****************************************************************************************/
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Updating a Model with Wave Functions");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(100, Delay, 0);
	glutMainLoop();
	return 0;
}
/******************************************************************************************************************/
