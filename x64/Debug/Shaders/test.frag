#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenText;

void main()
{
    vec2 uv = TexCoord;
    uv.y *= -1;
    vec4 outColor = texture(screenText, uv);

    FragColor = outColor;
} 