#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "GL/glew.h"

#include "ShaderProgram.h"

struct ScreenVertex
{
	glm::vec2 vertex;
	glm::vec2 uv;
};

class ScreenQuad
{
    
    private:
	GLuint vao, vbo, ebo;
    
    public:
	std::vector<ScreenVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<GLuint> textures;
    
	ScreenQuad(glm::vec2 pos, glm::vec2 size, glm::vec2* coords, int screenWidth, int screenHeight);
	ScreenQuad(glm::vec2 pos, glm::vec2 Screesize, int screenWidth, int screenHeight);
	ScreenQuad();
    ~ScreenQuad();
    
    void setQuad(glm::vec2 pos, glm::vec2 size, glm::vec2* coords, int screenWidth, int screenHeight);
	void setupQuad();
	void convertScreenPosToUV(glm::vec2& screenPos, int screenWidth, int screenHeight);
	void draw(ShaderProgram& program);
    
};

