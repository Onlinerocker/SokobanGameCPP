#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aGlowing;
layout (location = 3) in float aLayer;

out vec2 TexCoord;
out float Glowing;
out float Layer;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);
    TexCoord = aTexCoord;
    Glowing = aGlowing;
    Layer = aLayer;
}