#version 430 core

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec4 Position;
out vec3 Normal;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 model_matrix;

void main(void)
{
    Normal = normalize(mat3(view_matrix*model_matrix)* VertexNormal);
    Position = view_matrix*model_matrix *VertexPosition;
    gl_Position = projection_matrix* view_matrix*model_matrix *VertexPosition;
}
