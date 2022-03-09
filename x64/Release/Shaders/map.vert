#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aID;

out vec2 TexCoord;
out float ID;
out vec2 Pos;
out vec3 Color;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
    ID = aID;
    Pos = aPos;
	Color = aColor;
}