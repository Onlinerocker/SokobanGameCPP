#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform float alpha;
uniform float border;

void main()
{
    if(TexCoord.x > border && TexCoord.x < (1 - border) && TexCoord.y > border && TexCoord.y < (1 - border)) discard;
    FragColor = vec4(color.r, color.g, color.b, alpha);
} 