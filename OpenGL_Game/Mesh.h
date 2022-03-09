#pragma once
#include <glm/glm.hpp>
#include <GL\glew.h>

#include <vector>

#include "ShaderProgram.h"

enum TextureType
{
	diffuse = 0,
	specular = 1
};

struct Vertex
{
	glm::vec2 vertex;
	glm::vec2 uv;
};

struct VertexInstance
{
	glm::vec2 vertex;
};

struct VertexUI
{
	glm::vec2 vertex;
	glm::vec2 uv;
	float glowing;
	float layer;
};

struct VertexMap
{
	glm::vec2 vertex;
	glm::vec2 uv;
    glm::vec3 color;
	float id;
};

struct Texture
{
	GLuint id;
	TextureType type;
};

class Mesh
{
    
    private:
	GLuint vao, vbo, ebo, instanceVbo, instanceVboUv, instanceVboGlow, instanceVboTime, instanceVboColor;
	bool isVao, isVbo, isEbo, isInstanceVbo, isInstanceVboUv, isInstanceVboGlow, isInstanceVboTime,
    isInstanceVboColor;
    
	void createBuffers();
	void deleteBuffers();
	void setUpVertsInds(void* vertices, size_t vertSize, void* indices, size_t indSize);
    
    public:
	std::vector<Vertex> vertices;
	std::vector<VertexInstance> verticesInstance;
	std::vector<VertexUI> verticesUi;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	std::vector<VertexMap> verticesMap;
    
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	Mesh();
	~Mesh();
    
	void setupMesh();
	void setupInstancedMesh(const std::vector<glm::vec2>& offsets, const std::vector<glm::vec4>& uvs, const std::vector<float>& glows);
	void setupUiMesh();
	void setupParticleMesh(const glm::vec2* const offsets, const glm::vec4* const uvs, const float* const sizes, const float* const times, const glm::vec3* color, size_t count);
	void setupMeshMap();
    
	void draw(ShaderProgram& program);
	void drawArray(ShaderProgram& program);
	void drawInstanced(ShaderProgram& program, GLsizei instances);
    
    
};

