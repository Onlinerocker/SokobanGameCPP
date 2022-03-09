#include "Mesh.h"
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
    
    this->isVao = false;
    this->isVbo = false;
    this->isEbo = false;
    this->isInstanceVbo = false;
    this->isInstanceVboGlow = false;
    this->isInstanceVboUv = false;
    this->isInstanceVboTime = false;
    
    createBuffers();
	setupMesh();
}

Mesh::Mesh() 
{
    this->isVao = false;
    this->isVbo = false;
    this->isEbo = false;
    this->isInstanceVbo = false;
    this->isInstanceVboGlow = false;
    this->isInstanceVboUv = false;
    this->isInstanceVboTime = false;
    this->isInstanceVboColor = false;
    
    createBuffers();
}

Mesh::~Mesh()
{
    deleteBuffers();
}

void Mesh::deleteBuffers()
{
    if(isVbo) glDeleteBuffers(1, &vbo);
    if(isVao) glDeleteVertexArrays(1, &vao);
    if(isEbo) glDeleteBuffers(1, &ebo);
    if(isInstanceVbo) glDeleteBuffers(1, &instanceVbo);
    if (isInstanceVboUv) glDeleteBuffers(1, &instanceVboUv);
    if (isInstanceVboGlow) glDeleteBuffers(1, &instanceVboGlow);
    if (isInstanceVboTime) glDeleteBuffers(1, &instanceVboTime);
    if (isInstanceVboColor) glDeleteBuffers(1, &instanceVboColor);
}

void Mesh::createBuffers()
{
    glGenBuffers(1, &vbo); //create buffer
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    
    this->isVao = true;
    this->isVbo = true;
    this->isEbo = true;
}

void Mesh::setUpVertsInds(void* vertices, size_t vertSize, void* indices, size_t indSize)
{
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //bind buffer
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW); //load buffer
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW); //are the vertex and element buffer the same?
}

void Mesh::setupMesh()
{
    //if (vertices.empty()) return;
    setUpVertsInds(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(GLuint) * indices.size());
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)) ); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0); //unbind vertex array
}

void Mesh::setupMeshMap()
{
    //if (verticesMap.empty()) return;
    setUpVertsInds(verticesMap.data(), sizeof(VertexMap) * verticesMap.size(), indices.data(), sizeof(GLuint) * indices.size());
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexMap), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexMap), (void*)(offsetof(VertexMap, uv))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexMap), (void*)(offsetof(VertexMap, color))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexMap), (void*)(offsetof(VertexMap, id))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0); //unbind vertex array
}

void Mesh::setupInstancedMesh(const std::vector<glm::vec2>& offsets, const std::vector<glm::vec4>& uvs, const std::vector<float>& glows)
{
    //deleteBuffers();
    //createBuffers();
    
    //if (offsets.empty() || uvs.empty() || glows.empty() || verticesInstance.empty() || indices.empty()) return;

    if (!isInstanceVbo)
    {
        glGenBuffers(1, &instanceVbo);
        isInstanceVbo = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * offsets.size(), offsets.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!isInstanceVboUv)
    {
        glGenBuffers(1, &instanceVboUv);
        isInstanceVboUv = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboUv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!isInstanceVboGlow)
    {
        glGenBuffers(1, &instanceVboGlow);
        isInstanceVboGlow = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboGlow);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * glows.size(), glows.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //bind buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexInstance) * verticesInstance.size(), verticesInstance.data(), GL_STATIC_DRAW); //load buffer
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW); //are the vertex and element buffer the same?
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstance), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboUv);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(1, 1);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);
    
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboGlow);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1);
    
    glBindVertexArray(0); //unbind vertex array
}

void Mesh::setupUiMesh()
{
    setUpVertsInds(verticesUi.data(), sizeof(VertexUI) * verticesUi.size(), indices.data(), sizeof(GLuint) * indices.size());
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUI), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUI), (void*)(offsetof(VertexUI, uv))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(VertexUI), (void*)(offsetof(VertexUI, glowing)));
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexUI), (void*)(offsetof(VertexUI, layer)));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0); //unbind vertex array
}

void Mesh::setupParticleMesh(const glm::vec2* const offsets, const glm::vec4* const uvs, const float* const sizes, const float* const times, const glm::vec3* colors, size_t count)
{
    if (!isInstanceVbo)
    {
        glGenBuffers(1, &instanceVbo);
        isInstanceVbo = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * count, &offsets[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!isInstanceVboUv)
    {
        glGenBuffers(1, &instanceVboUv);
        isInstanceVboUv = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboUv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * count, &uvs[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!isInstanceVboGlow)
    {
        glGenBuffers(1, &instanceVboGlow);
        isInstanceVboGlow = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboGlow);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, &sizes[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!isInstanceVboTime)
    {
        glGenBuffers(1, &instanceVboTime);
        isInstanceVboTime = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboTime);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, &times[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!isInstanceVboColor)
    {
        glGenBuffers(1, &instanceVboColor);
        isInstanceVboColor = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboColor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, &colors[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //bind buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexInstance) * verticesInstance.size(), &verticesInstance[0], GL_STATIC_DRAW); //load buffer
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW); //are the vertex and element buffer the same?
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInstance), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboUv);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(1, 1);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);
    
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboGlow);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1);
    
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboTime);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(4, 1);
    
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVboColor);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(5, 1);
    
    glBindVertexArray(0); //unbind vertex array
}

void Mesh::draw(ShaderProgram& program)
{
    //bind textures
    
    for (int x = 0; x < textures.size(); x++) //TODO support variable amounts of each
    {
        const char* shaderName = textures[x].type == diffuse ? "diffuse" : "specular";
        
        //program.getAttribute(shaderName);
        program.setInt(shaderName, x);
        
        glActiveTexture(GL_TEXTURE0 + x);
        glBindTexture(GL_TEXTURE_2D, textures[x].id);
    }
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size() , GL_UNSIGNED_INT, 0);
}

void Mesh::drawArray(ShaderProgram& program)
{
    //bind textures
    
    for (int x = 0; x < textures.size(); x++) //TODO support variable amounts of each
    {
        const char* shaderName = textures[x].type == diffuse ? "diffuse" : "specular";
        
        //program.getAttribute(shaderName);
        program.setInt(shaderName, x);
        
        glActiveTexture(GL_TEXTURE0 + x);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures[x].id);
    }
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::drawInstanced(ShaderProgram& program, GLsizei instances)
{
    GLuint offset = 0;
    for (Texture t : textures) //TODO support variable amounts of each
    {
        const char* shaderName = t.type == diffuse ? "diffuse" : "specular";
        
        //program.getAttribute(shaderName);
        program.setInt(shaderName, offset);
        
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures[offset].id);
        
        offset++;
    }
    
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instances);
}