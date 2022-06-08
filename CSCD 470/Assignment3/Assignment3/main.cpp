#include <GL/glew.h> 
#include <math.h>
#include "GLM.h"
#include "Disk.h"
#include "Sphere.h"

//Note
//I have put the declarations for the sphere to shine in both shader files and in main.
//but sphere won't light up, only small section of it.

mat4 view(1.0f);
mat4 model(1.0f);
mat4 projection(1.0f);

GLuint programGouraud, programPhong;
float aspect = 0.0;
GLfloat angle = 0.0;
GLuint vao;
unsigned int vbo[1];
GLuint ebo;

/* Class GLM object*/
GLMmodel* objmodel = NULL;
//vec3* modifiedVerts = NULL;
//vec3* modifiedNorms = NULL;

// vectors that the program came with

bool show_line = false;
bool switch_shaders = false;
bool top_view = false;

vec4 light_position(10.0, 6.0, 8.0, 1.0);  // directional light source
GLfloat timeUpdate = 0.01;
GLfloat vscale = 0.0;

vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;

vec4 material_ambient(0.6, 0.5, 0.3, 1.0f);
vec4 material_diffuse(0.9, 0.5, 0.3, 1.0f);
vec4 material_specular(0.8, 0.8, 0.8, 1.0f);

vec4 lightintensity = vec4(0.9f, 0.9f, 0.9f, 1.0f);
float material_shininess = 50.0;

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
	/*vscale = 0.08 * cos(TWOPI * timeUpdate) + 0.06;

	for (int i = 0; i < count; i++)
	{
		ptr[i] = verts[i] + vscale * norms[i];
	}*/
}

void drawBox(vec3* vertices) {

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

	float center_x = (max_x + min_x) / 2;
	float center_y = (max_y + min_y) / 2;
	float center_z = (max_z + min_z) / 2;

	float width = abs(max_x - min_x);
	float height = abs(max_y - min_y);
	float depth = abs(max_z - min_z);

	float scale = max(depth, max(width, height));

	//moving points to center of the screen
	for (int i = 0; i < objmodel->numvertices; i++) {
		vertices[i].x -= center_x;
		vertices[i].y -= center_y;
		vertices[i].z -= center_z;

		vertices[i].x /= scale / 2;
		vertices[i].y /= scale / 2;
		vertices[i].z /= scale / 2;

		vertices[i].x *= 7;
		vertices[i].y *= 7;
		vertices[i].z *= 7;
	}
}

/********************************************************************************/

void setUniform(GLuint p) {
	glUseProgram(p);
	glUniform4fv(glGetUniformLocation(p, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(p, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(p, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	//glUniform4fv(glGetUniformLocation(p, "Spot.intensity"), 1, (GLfloat*)&lightintensity[0]);
	glUniform4fv(glGetUniformLocation(p, "Spot.position"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(p, "Shininess"), material_shininess);
	glUniform1f(glGetUniformLocation(p, "Spot_exponent"), 30.0f);
	glUniform1f(glGetUniformLocation(p, "Spot_cutoff"), 15.0f);
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
		drawBox(objmodel->vertices);
	}

	// Create and compile our GLSL program from the shaders

	programGouraud = initShaders("shader_gouraud.vs", "shader_gouraud.fs");
	programPhong = initShaders("shader.vs", "shader.fs");

	view = glm::lookAt(vec3(0.0f, 5.0f, 30.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	//view = glm::lookAt(vec3(0.0f, 37.5f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
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

	ambient_product = lightintensity * material_ambient;
	diffuse_product = lightintensity * material_diffuse;
	specular_product = lightintensity * material_specular;

	setUniform(programGouraud);
	setUniform(programPhong);

	createDisk();
	createSphere();

	glClearColor(0.0, 0.0, 0.0, 1.0);
}
/*******************************************************************************************/
void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	model = mat4(1.0f);
	projection = glm::perspective(radians(45.0f), aspect, 0.3f, 100.0f);

	GLuint program;

	if (switch_shaders) {
		program = programGouraud;
		glutSetWindowTitle("Modeling a Spot Light – Gouraud Shading");
	}
	else {
		program = programPhong;
		glutSetWindowTitle("Modeling a Spot Light - Phong Shading");
	}

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);

	light_position = vec4(10.0f * cos(radians(angle)), 10.0f, 10.0f * sin(radians(angle)), 1.0);
	vec3 spot_dir = mat3(view) * (vec3(-light_position));

	vec3 light_pos = view * light_position;
	glUniform3fv(glGetUniformLocation(program, "Spot_direction"), 1, (GLfloat*)&spot_dir);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_pos[0]);

	if (light_position.w == 0)
		light_pos = light_position;
	else
		light_pos = view * light_position;

	if (top_view)
		view = glm::lookAt(vec3(0.0f, 37.5f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
	else
		view = glm::lookAt(vec3(0.0f, 5.0f, 30.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	if (show_line)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

	model = mat4(1.0f) * translate(mat4(1.0f), vec3(0.0, -7.0, 0.0))* scale(mat4(1.0f), vec3(10.0, 0.0, 10.0));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	drawDisk();

	glUniform1f(glGetUniformLocation(program, "On"), 1);
	model = mat4(1.0f) * translate(mat4(1.0f), vec3(light_position));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	drawSphere();


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
	case 's':case 'S':
		switch_shaders = !switch_shaders;
		break;
	case 't':case 'T':
		top_view = !top_view;
		break;
	}
	glutPostRedisplay();
}
/**************************************************************************************/

void RotateLight(int n)
{
	angle += 5.0;
	glutPostRedisplay();
	glutTimerFunc(50, RotateLight, 0);
}

/****************************************************************************************/
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Modeling a Spot Light - Phong Shading");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(50, RotateLight, 0);
	glutMainLoop();
	return 0;
}
/******************************************************************************************************************/
