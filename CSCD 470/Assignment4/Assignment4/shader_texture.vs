#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 VertexTexCoord;


uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;


uniform vec4 LightPosition;


out vec4 eyeSpacePosition;
out vec3 eyeSpaceNormal;
out vec3 lightDirection;
out vec2 TexCoord;


void main(void)
{

eyeSpacePosition = view_matrix*model_matrix*position;
eyeSpaceNormal = mat3(view_matrix*model_matrix)*normal;
lightDirection = LightPosition.xyz - eyeSpacePosition.xyz;
TexCoord = VertexTexCoord;

gl_Position = projection_matrix*view_matrix*model_matrix*position;


}

