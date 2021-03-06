#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aIndex;

out vec2 TexCoord;
out float Index;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);
    TexCoord = aTexCoord;
    Index = aIndex;
}