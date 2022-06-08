#version 430

in vec3 ReflectDir;
in vec2 TexCoord;

uniform samplerCube CubeMapTex;
uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform bool DrawSkyBox;
uniform bool DrawPedestal;

uniform bool TextureModel;
uniform vec4 MaterialColor;

uniform float ReflectFactor;

layout( location = 0 ) out vec4 FragColor;

void main() {

    // Access the cube map texture

       vec4 texColor = texture(Tex1, TexCoord);
       vec4 cubeMapColor = texture(CubeMapTex, ReflectDir);

        if (DrawSkyBox)
        FragColor = cubeMapColor;
       else{
            FragColor = mix(cubeMapColor, MaterialColor, 0);
            if( !DrawPedestal ){
                if (TextureModel){
                    FragColor = mix(texColor, cubeMapColor, ReflectFactor); //texture gold
                }
                    else{
                        FragColor = mix(MaterialColor, cubeMapColor, ReflectFactor);//color
                    }
                }
                else{
                    FragColor = texture(Tex2, TexCoord); //text for pedestal
                }
                //FragColor = mix(cubeMapColor, MaterialColor, 0);
       }
 
  
}