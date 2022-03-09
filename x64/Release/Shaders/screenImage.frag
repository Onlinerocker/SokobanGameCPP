#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenText;
uniform vec3 color;

void main()
{
    vec2 uv = TexCoord;
    uv.y *= -1;
    vec4 outColor = texture(screenText, uv);
	outColor = vec4(outColor.rgb * color, outColor.a);

    FragColor = outColor;
} 