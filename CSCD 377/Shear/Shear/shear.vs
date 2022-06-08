#version 430 core

layout (location = 0) in vec4 vPosition;

uniform mat4 model_transformation;


void main(){

     gl_Position = model_transformation*vPosition;
     
}
