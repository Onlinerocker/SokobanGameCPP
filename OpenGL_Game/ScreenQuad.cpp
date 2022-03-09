#include "ScreenQuad.h"

#include <iostream>

//takes in position and size in screen space (actual pixel location and size, not UV)
ScreenQuad::ScreenQuad(glm::vec2 pos, glm::vec2 size, int screenWidth, int screenHeight)
{
    glm::vec2 coords[4] = { glm::vec2(0), glm::vec2(0, 1), glm::vec2(1,1), glm::vec2(1,0) };
    setQuad(pos, size, coords, screenWidth, screenHeight);
}

ScreenQuad::ScreenQuad(glm::vec2 pos, glm::vec2 size, glm::vec2* coords, int screenWidth, int screenHeight)
{
    setQuad(pos, size, coords, screenWidth, screenHeight);
}

//default takes up entire screen
ScreenQuad::ScreenQuad()
{
    indices = { 0, 1, 2, 0, 2, 3 };
    
    ScreenVertex v;
    v.vertex = glm::vec2(-1, -1);
    
    v.uv = glm::vec2(0,0);
    vertices.push_back(v);
    
    v.vertex = glm::vec2(-1, 1);
    v.uv = glm::vec2(0, 1);
    vertices.push_back(v);
    
    v.vertex = glm::vec2(1, 1);
    v.uv = v.vertex;
    vertices.push_back(v);
    
    v.vertex = glm::vec2(1, -1);
    v.uv = glm::vec2(1, 0);
    vertices.push_back(v);
    
    setupQuad();
}

ScreenQuad::~ScreenQuad()
{
}

void ScreenQuad::setQuad(glm::vec2 pos, glm::vec2 size, glm::vec2* coords, int screenWidth, int screenHeight)
{
    vertices.clear();
    indices.clear();
    
    indices = { 0, 1, 2, 0, 2, 3 };
    
    ScreenVertex v;
    v.vertex = pos;
    convertScreenPosToUV(v.vertex, screenWidth, screenHeight);
    v.uv = coords[0];
    vertices.push_back(v);
    
    v.vertex = pos + glm::vec2(0, size.y);
    convertScreenPosToUV(v.vertex, screenWidth, screenHeight);
    v.uv = coords[1];
    vertices.push_back(v);
    
    v.vertex = pos + size;
    convertScreenPosToUV(v.vertex, screenWidth, screenHeight);
    v.uv = coords[2];
    vertices.push_back(v);
    
    v.vertex = pos + glm::vec2(size.x, 0);
    convertScreenPosToUV(v.vertex, screenWidth, screenHeight);
    v.uv = coords[3];
    vertices.push_back(v);
    
    setupQuad();
}

void ScreenQuad::setupQuad()
{
    glGenBuffers(1, &vbo); //create buffer
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //bind buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW); //load buffer
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW); //are the vertex and element buffer the same?
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenVertex), (void*)(offsetof(ScreenVertex, uv))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0); //unbind vertex array
}

void ScreenQuad::convertScreenPosToUV(glm::vec2 &pos, int screenWidth, int screenHeight)
{
    pos.x /= screenWidth;
    pos.y /= screenHeight;
    
    pos.x = -1 + (pos.x * 2.0);
    pos.y = -1 + (pos.y * 2.0);
}

void ScreenQuad::draw(ShaderProgram& program)
{
    //bind textures
    
    GLuint offset = 0;
    
    if (textures.size() > 0)
    {
        program.setInt("screenText", offset);
        
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, textures[offset]);
    }
    
    if (textures.size() > 1)
    {
        offset = 1;
        program.setInt("randomText", offset);
        
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, textures[offset]);
    }
    
    if (textures.size() > 2)
    {
        offset = 2;
        
        program.setInt("noiseText", offset);
        
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, textures[offset]);
    }
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}