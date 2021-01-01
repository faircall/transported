#version 330 core
out vec4 frag_color;
in vec3 our_color;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main()
{
    
    frag_color = mix(texture(ourTexture, texCoord), texture(ourTexture2, texCoord), 0.2f); 
}