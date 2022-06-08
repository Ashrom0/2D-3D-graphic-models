#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 VertexTexCoord;;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec4 LightPosition;

out vec4 EyeSpacePosition;
out vec3 EyeSpaceNormal;
out vec3 LightDirection;
out vec2 TexCoord;

void main(void)
{
	EyeSpacePosition = view_matrix * model_matrix * position;
	EyeSpaceNormal = mat3(view_matrix * model_matrix) * normal;
	LightDirection = LightPosition.xyz - EyeSpacePosition.xyz;
	TexCoord = VertexTexCoord;;

	gl_Position = projection_matrix * view_matrix * model_matrix * position;
}