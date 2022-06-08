#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

uniform mat4 model;
uniform mat4 view;

uniform mat4 projection;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

uniform vec3 Spot_direction;
uniform float Spot_cutoff;
uniform float Spot_exponent;

uniform bool On;

out vec4 Color;

void main()
{
   vec3 fN = mat3(view*model)*VertexNormal;
   vec4 fE = view*model*vec4(VertexPosition, 1.0);
   vec3 fL = LightPosition.xyz - fE.xyz;
   
   gl_Position = projection*view*model * vec4(VertexPosition,1.0);

    // Normalize the input lighting vectors
    vec3 N = normalize(fN);
    vec3 E = normalize(-fE.xyz); //Camera
    vec3 L = normalize(fL);

    vec3 H = normalize(L+E);

    vec4 ambient = AmbientProduct;
    float Kd = max(dot(L,N), 0.0);
    vec4 diffuse = Kd*DiffuseProduct;

    float Ks = pow(max(dot(N, H), 0.0), Shininess);
    vec4 specular = Ks*SpecularProduct;

    float beta = acos(dot(-L, normalize(Spot_direction)));
    float alpha = radians(Spot_cutoff);

    if (On){
    ambient; + diffuse + specular;
    }
    else{
    if( beta < alpha ) {
        float spotFactor = pow( dot(-L, Spot_direction), Spot_exponent );
        ambient + spotFactor*( diffuse + specular);
    } else 
        ambient;
    }

    if (beta < alpha){
        float spot_Factor = pow(cos(beta), Spot_exponent );
        Color = ambient + spot_Factor * (diffuse + specular);
    }
    else{
        Color = ambient;
    }

    Color.a = 1.0;
}
