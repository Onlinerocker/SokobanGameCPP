#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

void main()
{
	vec4 outColor = mix(vec4(0.05, 0.05, 0.05, 0.75), vec4(0.1, 0.1, 0.1, 0.75), 0.0);
	FragColor = outColor;
} 