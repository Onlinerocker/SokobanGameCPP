#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float timeX;
uniform float timeY;
uniform float fadeTime;
uniform sampler2D screenText;
uniform sampler2D randomText;

void main()
{
    vec2 uv = TexCoord;
    vec4 outColor = mix(vec4(0.9), texture(screenText, uv), 0.9);
    outColor = mix(vec4(0.7), outColor, smoothstep(1.0, 0.7, TexCoord.y));
    //outColor = texture(snowText, uv);
    uv += vec2(-timeX, -timeY);
    outColor += 1*texture(randomText, uv);

    float t = fadeTime;
    float v = smoothstep(t, t+0.01, TexCoord.x + 0.01);
    outColor = mix(vec4(0,0,0,1), outColor, v);
    FragColor = outColor;
} 