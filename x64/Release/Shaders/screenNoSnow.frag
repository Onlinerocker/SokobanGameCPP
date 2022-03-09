#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenText;
uniform sampler2D randomText;
uniform float fadeTime;

const float NOISE_GRANULARITY = 0.5/255.0;

void main()
{
    vec2 uvVig = TexCoord;
    uvVig *= 1.0 - uvVig.xy;
    float vig = uvVig.x * uvVig.y * 20.0;
    vig = pow(vig, 0.5);

    vec4 c = texture(screenText, TexCoord);

    float t = fadeTime;
    t = (t - 1.0);
    t *= t;
    t = 1 - clamp(t, 0.0, 1.0);
    FragColor = mix(c, vec4(0,0,0,1), t);
}