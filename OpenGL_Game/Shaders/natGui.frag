#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in float Glowing;
in float Layer;

uniform sampler2DArray diffuse;

void main()
{
    vec2 uv = TexCoord;
    vec4 outColor = texture(diffuse, vec3(uv.xy, Layer));

    if(Glowing > 0.5f) outColor *= 2.0f;

    FragColor = outColor;
} 