#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;


out vec4 fColor;
void main(void)
{
    gl_Position = (projection_matrix*view_matrix*model_matrix*position);
fColor = color;
}
