#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float time;
uniform sampler2D screenText;
uniform sampler2D randomText;

void main()
{
    vec2 uv = TexCoord;
    uv.y *= -1;
    vec4 outColor = mix(texture(screenText, uv), texture(randomText, uv), smoothstep(0.0, 1.0, time));

    FragColor = outColor;
} 