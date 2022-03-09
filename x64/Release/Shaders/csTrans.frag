#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
uniform float fadeTime;

void main()
{
    float t = 1.0 - fadeTime;
    t = (t - 1.0);
    t *= t;
    t = 1 - clamp(t, 0.0, 1.0);
    FragColor = mix(vec4(0,0,0,0), vec4(0,0,0,1), t);
}