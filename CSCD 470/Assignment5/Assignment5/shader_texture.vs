#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 VertexTexCoord;


uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;


uniform vec4 LightPosition;

uniform float Timer;

out vec4 eyeSpacePosition;
out vec3 eyeSpaceNormal;
out vec3 lightDirection;
out vec2 TexCoord;
out vec2 TexCoordOne;
out vec2 TexCoordTwo;


void main(void)
{

mat2 m = mat2(cos(Timer), sin(Timer), -sin(Timer), cos(Timer));

TexCoordOne = VertexTexCoord * m;

mat2 m2 = transpose(m);
TexCoordTwo = VertexTexCoord * m2;

eyeSpacePosition = view_matrix*model_matrix*position;
eyeSpaceNormal = mat3(view_matrix*model_matrix)*normal;
lightDirection = LightPosition.xyz - eyeSpacePosition.xyz;
TexCoord = VertexTexCoord;

gl_Position = projection_matrix*view_matrix*model_matrix*position;


}

