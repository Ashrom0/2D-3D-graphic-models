#version 430 core


in vec4 eyeSpacePosition;
in vec3 eyeSpaceNormal;
in vec3 lightDirection;

uniform vec3 Ambient;
in vec2 TexCoord;

uniform sampler2D Tex1;


uniform vec3 LightColor;
out vec4 finalColor;


void main(void)
{

vec3 N = normalize(eyeSpaceNormal);
vec3 L = normalize(lightDirection);
vec3 V= normalize( vec3(0.0) - vec3(eyeSpacePosition));
vec3 H = normalize(L+V);

vec3 diffuse = LightColor*(max(0.0, dot(N, L)));
vec3 specular = LightColor*(pow(max(0.0, dot(N, H)), 50.0) );

vec4 texColor = texture( Tex1, TexCoord );

vec3 scatteredColor =  Ambient + diffuse;

finalColor = min( vec4(vec3(texColor)*scatteredColor + specular, 1.0), vec4(1.0));




}
