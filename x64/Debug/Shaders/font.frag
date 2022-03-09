#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in float Index;

uniform sampler2D screenText;
uniform vec3 color;
uniform float time;

void main()
{
    vec2 uv = TexCoord;
    vec4 outColor = texture(screenText, uv);

    outColor = mix(vec4(0), vec4(color.rgb, 1), outColor.r);
    outColor.a *= smoothstep(Index - 2, Index + 2, time);

    FragColor = outColor;
} 