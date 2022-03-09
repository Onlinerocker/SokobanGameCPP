#pragma once

#include "ShaderProgram.h"

#include "stb_truetype.h"
#include "GL/glew.h"
#include "glm/glm.hpp"

#include <SDL.h>
#include "SDL_ttf.h"
#include <unordered_map>

struct GlyphData
{
    int minX;
    int maxX;
    int minY;
    int maxY;
    int adv;
    
    int posX;
    int posY;
    
    int w, h;
    
    float uvxMin;
    float uvxMax;
    float uvyMin;
    float uvyMax;
};

class TrueTypeFont
{
    private:
	unsigned char* ttf_buffer;
	unsigned char temp_bitmap[512 * 512];
    
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    
	void initFont(const char* fontPath);
    void initUnicodeFont(const char* fontPath, int fontPoint, SDL_Color color);
    
    public:
    /* Unicode font */
    int x, y;
    TTF_Font* uniFont;
    SDL_Color color;
    GLuint atlasBuffer;
    std::unordered_map<Uint16, GlyphData> glyphCache;
    ShaderProgram* glyphProg;
    /* END Unicode font stuff */
    
    int fontPoint;
    int fontHeight;
    
	GLuint ftex, uniTexture;
    
	TrueTypeFont(const char* path);
    TrueTypeFont(const char* path, SDL_Color color, int fontPoint, ShaderProgram* prog);
    TrueTypeFont(const char* path, SDL_Color color, ShaderProgram* prog);
    TrueTypeFont();
	~TrueTypeFont();
    
	void getTexCoords(char c, float* x, float* y, glm::vec2* coords, glm::vec2* vecs);
    GlyphData getGlyphInfo(Uint16 glyph);
    
	float getCharLen(char c);
    
};

