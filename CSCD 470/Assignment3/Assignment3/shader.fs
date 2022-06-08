#version 430

in vec3 fN;
in vec3 fL;
in vec4 fE;


out vec4 fColor;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

uniform vec3 Spot_direction;
uniform float Spot_cutoff;
uniform float Spot_exponent;

uniform bool On;

void main(void)
{

// Normalize the input lighting vectors
vec3 N = normalize(fN);
vec3 E = normalize(-fE.xyz);
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

if(beta < alpha){
    float spot_factor = pow(cos(beta), Spot_exponent);
    fColor = ambient + spot_factor*(diffuse + specular);
    
}

else{
    fColor = ambient;

}
fColor.a = 1.0;

}


