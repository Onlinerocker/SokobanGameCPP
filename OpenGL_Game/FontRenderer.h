#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "GL/glew.h"

#include "TrueTypeFont.h"
#include "ShaderProgram.h"

struct FontVertex
{
	glm::vec2 vertex;
	glm::vec2 uv;
	glm::float32 ind;
};

class FontRenderer
{
    private:
	GLuint vao, vbo, ebo;
    
	std::vector<FontVertex> verts;
	std::vector<GLuint> indices;
	GLuint texture, uniTexture;
	
	TrueTypeFont* font;
	float xPos, yPos;
	int lineLength, lineHeight, screenWidth, screenHeight;
    
	void createBuffers();
	void setupBuffers();
	void convertScreenPosToUV(glm::vec2& screenPos, int screenWidth, int screenHeight);
	void convertFloatToUV(float& pos, int size);
    
    public:
	FontRenderer(const char* text, TrueTypeFont* font, float x, float y, int lineLength, int lineHeight, int screenWidth, int screenHeight);
    
	void draw(ShaderProgram& program);
	void setText(const char* text);
    void setTextUtf16(const Uint16* text, int len);
    void setTextUtf16(const Uint16* text, int len, bool flipBytes, float scale);
    GLuint createUtf8Image(const char* text, int* w, int* h);
};

