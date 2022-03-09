#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aTexCoord;
layout (location = 2) in vec2 aOffset;
layout (location = 3) in float aGlowing;
layout (location = 4) in float aTime;
layout (location = 5) in vec3 aColor;

out vec2 TexCoord;
out float Glowing;
out float Time;
out vec3 Color;

uniform mat4 transform;

void main()
{
    vec2 p = aPos.xy * aGlowing;
    p -= vec2(9.0, 5.0);
    gl_Position = transform * vec4(p + aOffset, aPos.z, 1.0);
    Glowing = aGlowing;
    Time = aTime;
    Color = aColor;

    if(gl_VertexID == 0) TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    if(gl_VertexID == 1) TexCoord = vec2(aTexCoord.x, aTexCoord.w);
    if(gl_VertexID == 2) TexCoord = vec2(aTexCoord.z, aTexCoord.w);
    if(gl_VertexID == 3) TexCoord = vec2(aTexCoord.z, aTexCoord.y);
}