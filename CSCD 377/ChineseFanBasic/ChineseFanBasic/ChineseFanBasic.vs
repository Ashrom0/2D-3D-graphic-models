#version 430 core

layout (location = 0) in vec4 vPosition;
uniform mat4 model_matrix;
layout (location = 1) in vec4 vColor;
out vec4 color;

void main(){

     gl_Position = model_matrix*vPosition;
     color = vColor;
}
