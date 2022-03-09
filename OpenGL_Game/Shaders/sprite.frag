#version 330 core
#define PI 3.14159265359

out vec4 FragColor;

in vec2 TexCoord;

uniform float time;
uniform sampler2D diffuse;
uniform vec4 color;

void main()
{
    vec2 uv = TexCoord;
    //uv.y *= -1;

	float ti = time*2.0*PI;
	uv.x += 0.2*sin(ti + uv.y);
    vec4 outColor = texture(diffuse, uv);
	outColor *= color;

    FragColor = outColor;
} 