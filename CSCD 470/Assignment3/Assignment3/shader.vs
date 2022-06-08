#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;


out vec3 fN;
out vec4 fE;
out vec3 fL;

uniform mat4 model;
uniform mat4 view;

uniform mat4 projection;
uniform vec4 LightPosition;



void main()
{
   fN = mat3(view*model)*VertexNormal;
   fE = view*model*vec4(VertexPosition, 1.0);
   fL = LightPosition.xyz - fE.xyz;
   
   gl_Position = projection*view*model * vec4(VertexPosition,1.0);
}
