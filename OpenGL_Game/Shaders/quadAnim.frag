#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float animTime;
uniform float duration;
uniform float frames;
uniform float uvOffset;
uniform bool xFlipped;
uniform sampler2D diffuse;


void main()
{
    vec2 uv = TexCoord;

    uv.x *= xFlipped ? -1 : 1;
    uv.y += uvOffset;

    float t = animTime;

    t = mod(animTime, duration);
    uv.x += floor((t/duration)*frames) * (1.0 / frames);

    vec4 outColor = texture(diffuse, uv);

    if(outColor.a < 0.1) discard;

    //float div = clamp(dot(normal, normalize(vec3(5.0, 5.0, 5.0))), 0, 1);
    //outColor *= div;

    FragColor = vec4(outColor.rgb*0.8, outColor.a);
} 