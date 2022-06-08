#include "ChineseFanBasic.h"

GLuint  vao;
GLuint  vbo[2];
GLuint  ebo;
GLuint v, f;
GLboolean show_line = false;
GLboolean vertex_update = false;

GLboolean multi_model = false;
GLboolean scale_model = false;
GLfloat rotate_model = false;

GLfloat theta = 0.0;
GLfloat angle = 0.0; //angles

//uniform variables
mat4 scale_matrix(1.0f);
mat4 translate_matrix(1.0f);
mat4 rotate_matrix(1.0f);
mat4 model_matrix(1.0f);

GLuint model_loc;

char* ReadFile(const char* filename);
GLuint initShaders(char* v_shader, char* f_shader);
void init();
void display(void);
void DrawFan();

/************************************************/

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

GLuint initShaders(const char* v_shader, const char* f_shader) {

	GLuint p = glCreateProgram();

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

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
void init() {

	GLuint program = initShaders("ChineseFanBasic.vs", "ChineseFanBasic.fs");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(2, vbo);  // you need to have two buffer objects if color is added
	
	// For the 1st attribute - vertex positions

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); //vertex color
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	model_loc = glGetUniformLocation(program, "model_matrix");
}
/*******************************************************/
void Keyboard(unsigned char key, int x, int y) {

	switch (key) {

	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w': case 'W':
		show_line = !show_line;
		break;
	case 's': case 'S':
		scale_model = !scale_model;
		break;
	case 'm': case 'M':
		multi_model = !multi_model;
		break;
	case 'r': case 'R':
		rotate_model = !rotate_model;
		break;
	}
	glutPostRedisplay();
}


/********************************************/

void Transform(int n) //the "glutTimerFunc"
{

	
	switch (n) {
	case 1:
		vertex_update = !vertex_update;
		if (rotate_model)
			angle += 5.0f;
		glutPostRedisplay();
		glutTimerFunc(500, Transform, 1);
		break;

	case 2:
		if(rotate_model)
			theta += 5.0f;
		glutPostRedisplay();
		glutTimerFunc(100, Transform, 2);
		break;
	}
}


/**********************************************************/

void DrawFan() {
	if (show_line)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT, GL_FILL);

	glBindVertexArray(vao);

	//scaling
	if (scale_model) {
		scale_matrix = scale(mat4(1.0), vec3(0.25, 0.25, 1.0));
		glPointSize(10.0);
	}

	else {
		scale_matrix = mat4(1.0f);
		glPointSize(20.0);
	}

	translate_matrix = mat4(1.0f);
	rotate_matrix = rotate(mat4(1.0f), radians(angle), vec3(0, 0, 1));

	model_matrix = translate_matrix * rotate_matrix * scale_matrix;
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);



	// add codes on updating vertices
	if (vertex_update) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_update), vertices_update);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
	}

	//center
	rotate_matrix = rotate(mat4(1.0f), radians(angle), vec3(0.0, 0.0, 1.0));
	model_matrix = rotate_matrix * scale_matrix;
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glBindVertexArray(vao);
	/*glDrawArrays(GL_TRIANGLES, 0, 24);
	glPolygonMode(GL_FRONT, GL_LINE);*/
	glDrawArrays(GL_POINTS, 0, 24);
	glDrawArrays(GL_TRIANGLES, 0, 24);

	//multiple
	if (multi_model && scale_model) {
		for (int i = 0; i < 6; i++) {
			translate_matrix = translate(mat4(1.0f), vec3(0.6 * cos(radians(60.0f * i + theta)), 0.6 * sin(radians(60.0f * i + theta)), 0.0));
			model_matrix = translate_matrix * rotate_matrix * scale_matrix;
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
			/*glDrawArrays(GL_TRIANGLES, 0, 24);
			glPolygonMode(GL_FRONT, GL_LINE);*/
			glDrawArrays(GL_POINTS, 0, 24);
			glDrawArrays(GL_TRIANGLES, 0, 24);

		}
	}



	glBindVertexArray(vao);
}
/**********************************************************/
void display(void) {

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glPointSize(20.0);

	glLineWidth(5.0);
	
	DrawFan();
	
	glFlush();

}

/*******************************************************/

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Chinese fan");

	if (glewInit()) {
		printf("%s\n", "Unable to initialize GLEW ...");
	}

	init();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(display);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(500, Transform, 1);
	glutTimerFunc(100, Transform, 2);
	glutMainLoop();

	return 0;

}

/*******************************************************/