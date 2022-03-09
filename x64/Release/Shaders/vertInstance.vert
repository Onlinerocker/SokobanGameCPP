#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aTexCoord;
layout (location = 2) in vec2 aOffset;
layout (location = 3) in float aGlowing;

out vec2 TexCoord;
out float Glowing;
out float ID;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos.xy + (aOffset * 17.0), aPos.z, 1.0);
    Glowing = aGlowing;
    ID = aTexCoord.w;

    if(gl_VertexID == 0) TexCoord = vec2(0, 0);
    if(gl_VertexID == 1) TexCoord = vec2(0, 1);
    if(gl_VertexID == 2) TexCoord = vec2(1, 1);
    if(gl_VertexID == 3) TexCoord = vec2(1, 0);
}