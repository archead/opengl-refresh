#version 460 core
out vec4 FragColor;
//in vec2 TexCoord;

//uniform sampler2D texture1;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
   //FragColor = texture(texture1, TexCoord);
   FragColor = vec4(lightColor * objectColor, 1.0f);
};