#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Glowing;
in float Time;
in vec3 Color;

uniform sampler2DArray diffuse;
uniform sampler2D specular;

void main()
{
    vec4 outColor = texture(diffuse, vec3(TexCoord.xy, 83.0));
    vec3 c = outColor.rgb * Color;//normalize(outColor.rgb);
    //c = vec3(0.06, 0.06, 0.06);
    
    float t = (0.5 - Time)/0.5;
    t = (t - 1.0);
    t *= t;
    t = 1 - clamp(t, 0.0, 1.0);
    outColor.a *= mix(0.0f, 1.0f, t);

    FragColor = vec4(c.rgb, outColor.a);
} 