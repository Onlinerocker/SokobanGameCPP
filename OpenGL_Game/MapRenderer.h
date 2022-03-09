#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "Map.h"
#include "Mesh.h"
#include "ShaderProgram.h"

class MapRenderer
{
    
    private:
	/*GLuint vao, vbo, ebo;

	//std::vector<FontVertex> verts;
	std::vector<GLuint> indices;
	GLuint texture;

	void createBuffers();
	void setupBuffers();*/
    
	GLuint texture;
    
	Map* map;
    
	Mesh mesh;
	Mesh mesh1;
	Mesh meshTop;
	Mesh meshDynamic; 
    
	void addQuad(Mesh* mesh, float x, float& curX, float curY, glm::vec2 pos, float tileSize, glm::vec2 uvMin, glm::vec2 uvMax, unsigned long& ind);
	void addQuadInstance(Mesh* mesh, float xInd, float& x, float y, glm::vec2 pos, float tileSize, unsigned long& ind);
	void addQuadMap(Mesh* mesh, float xInd, float& x, float y, glm::vec2 pos, float tileSize, unsigned long& ind, float id, glm::vec3 color);
    
    public:
	MapRenderer(Map* map, GLuint texture);
    
	Mesh meshParticle;
    
	void refresh();
    
	void draw(ShaderProgram& program);
	void drawTop(ShaderProgram& program);
	void drawDynamic(ShaderProgram& proram);
    
	void initMeshParticle(float size);
	//void setupMeshParticle(const std::vector<glm::vec2>& offsets, const std::vector<glm::vec4>& uvs, const std::vector<float>& glows);
	void setupMeshParticle(const glm::vec2* const offsets, const glm::vec4* const uvs, const float* const sizes, const float* const times, const glm::vec3* colors, size_t count);
};

