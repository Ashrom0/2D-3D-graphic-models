#version 430 core


in vec3 lightDirection;
in vec3 ViewDir;
uniform vec3 Ambient;
in vec2 TexCoord;
in vec3 eyeSpaceNormal;

uniform sampler2D EarthTex;
uniform sampler2D NormalTex;
uniform sampler2D MarsTex;
uniform sampler2D NormalTex2;

uniform bool NormalMapped;

uniform bool TexChange;
uniform bool MapChange;

uniform vec3 LightColor;
out vec4 finalColor;


void main(void)
{

   vec3 normal;

if (NormalMapped){
   if (MapChange){
       normal = texture( NormalTex2, TexCoord ).xyz; //mars
   }
   else{
       normal = texture( NormalTex, TexCoord ).xyz; //earth
   }
   normal = 2.0*normal - 1.0;

}

else 
   normal = eyeSpaceNormal;

   vec4 texColor = texture( EarthTex, TexCoord );
   vec4 texColor2 = texture( MarsTex, TexCoord );
   
   vec3 L = lightDirection;
   vec3 r = reflect( -L, normal);

   float diffuse = max(0.0, dot(normal, L) ) ;

   vec3 spec = vec3(0.0);
    if( diffuse > 0.0 )
        spec = LightColor *pow( max( dot(r,ViewDir), 0.0 ), 50.0 );


   vec3 scatteredColor =  Ambient + diffuse*LightColor;

   if (TexChange){
       finalColor = min( vec4(vec3(texColor2)*scatteredColor + spec, 1.0), vec4(1.0)); //mars
   }
   else{
       finalColor = min( vec4(vec3(texColor)*scatteredColor + spec, 1.0), vec4(1.0)); //earth
   }

   //finalColor = min( vec4(vec3(texColor)*scatteredColor + spec, 1.0), vec4(1.0));


}
