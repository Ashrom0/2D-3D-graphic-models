#version 430 core

in vec4 Position;
in vec3 Normal;

uniform vec4 LightPosition;  // Light position in eye coords.

out vec4 color;

void main(void)
{
   
   vec3 s = normalize(vec3(LightPosition) - vec3(Position) );
   float intensity = max( dot(s,Normal), 0.0 );
     
   if (intensity > 0.95)
     color = vec4(1.0,0.5,0.5,1.0);
     else if (intensity > 0.75)
     color = vec4(0.8,0.4,0.4,1.0);
    else if (intensity > 0.5)
     color = vec4(0.6,0.3,0.3,1.0);
    else if (intensity > 0.25)
     color = vec4(0.4,0.2,0.2,1.0);
    else
     color = vec4(0.2,0.1,0.1,1.0);


}
