#version 430 core

uniform vec3 Ambient;
uniform vec3 MaterialColor;

uniform sampler2D Tex1;

in vec4 EyeSpacePosition;
in vec3 EyeSpaceNormal;
in vec3 LightDirection;

in vec2 TexCoord;

uniform vec3 LightColor;
out vec4 finalColor;

void main(void)
{
    vec3 N = normalize(EyeSpaceNormal);
    vec3 L = normalize(LightDirection);
    vec3 V = normalize(vec3(0.0) - vec3(EyeSpacePosition));
    vec3 H = normalize(L + V);

    vec3 diffuse = LightColor*(max(0.0, dot(N, L)));
    vec3 specular = LightColor*(pow(max(0.0, dot(N, H)), 50.0) );

    vec4 TexColor = texture(Tex1, TexCoord);

    vec3 scatteredColor =  Ambient + diffuse;

    finalColor = min( vec4(vec3(TexColor)*scatteredColor + specular, 1.0), vec4(1.0));
}