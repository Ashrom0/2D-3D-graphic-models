#version 430 core


in vec4 eyeSpacePosition;
in vec3 eyeSpaceNormal;
in vec3 lightDirection;

uniform vec3 Ambient;
in vec2 TexCoord;
in vec2 TexCoordOne;
in vec2 TexCoordTwo;

uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform sampler2D Tex3;
uniform sampler2D Tex4;

uniform vec3 LightColor;
out vec4 finalColor;

uniform bool On;


void main(void)
{

vec3 N = normalize(eyeSpaceNormal);
vec3 L = normalize(lightDirection);
vec3 V= normalize( vec3(0.0) - vec3(eyeSpacePosition));
vec3 H = normalize(L+V);

vec3 diffuse;
vec3 specular;
vec4 texColor;

if (On){
    if(gl_FrontFacing){
        diffuse = LightColor*(max(0.0, dot(N, L)));
        specular = LightColor*(pow(max(0.0, dot(N, H)), 50.0) );
        
        vec4 TexColorOne = texture(Tex1, TexCoordOne);
        vec4 TexColorTwo = texture(Tex2, TexCoordTwo);
        
        texColor = TexColorOne + TexColorTwo;
    }
    
    else{
        diffuse = LightColor*(max(0.0, dot(-N, L)));
        specular = LightColor*(pow(max(0.0, dot(-N, H)), 50.0) );
    
        vec4 TexColorThree = texture(Tex3, TexCoord);
        texColor = TexColorThree;
    }
}

else{
    vec4 TexColorFour = texture(Tex4, TexCoord);
    texColor = TexColorFour;
}



vec3 scatteredColor =  Ambient + diffuse;

finalColor = min( vec4(vec3(texColor)*scatteredColor + specular, 1.0), vec4(1.0));




}
