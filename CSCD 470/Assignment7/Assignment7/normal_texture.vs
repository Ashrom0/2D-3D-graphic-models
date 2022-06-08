#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform bool NormalMapped;
uniform vec4 LightPosition;

out vec3 lightDirection;
out vec2 TexCoord;
out vec3 ViewDir;

out vec3 eyeSpaceNormal;

void main(void){

mat4 ModelViewMatrix = view_matrix*model_matrix;
mat3 normal_matrix = mat3(transpose(inverse(view_matrix*model_matrix)));

eyeSpaceNormal = normalize(normal_matrix*normal);

/*********************/
 // Transform normal and tangent to eye space
    vec3 norm = normalize( normal_matrix * normal);
    vec3 tang = normalize( normal_matrix * vec3(VertexTangent) )*VertexTangent.w;
    // Compute the binormal
    vec3 binormal = normalize( cross( norm, tang ) );

    // Matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
        tang.x, binormal.x, norm.x,
        tang.y, binormal.y, norm.y,
        tang.z, binormal.z, norm.z ) ;

    // Transform light direction and view direction to tangent space
    vec3 pos = vec3( ModelViewMatrix * vec4(position, 1.0) );

if (NormalMapped)
    lightDirection = normalize( toObjectLocal * (LightPosition.xyz - pos) );
else
    lightDirection = normalize(LightPosition.xyz - pos);

if (NormalMapped)
    ViewDir = normalize(toObjectLocal *(-pos));
else
	
    ViewDir = normalize(-pos);
/********************/
TexCoord = VertexTexCoord;

gl_Position = projection_matrix*view_matrix*model_matrix*vec4(position, 1.0);


}

