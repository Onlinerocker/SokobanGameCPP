#include "TrueTypeFont.h"
#include <cstdio>
#include <iostream>
#include <fstream>

#include "ScreenQuad.h"

TrueTypeFont::TrueTypeFont(const char* fontPath)
{
    initFont(fontPath);
}

TrueTypeFont::TrueTypeFont(const char* path, SDL_Color color, int fontPoint, ShaderProgram* prog)
{
    this->glyphProg = prog;
    x = 0;
    y = 0;
    this->fontPoint = fontPoint;
    
    initUnicodeFont(path, fontPoint, color);
    
    /* Create framebuffer for atlas */
    glGenFramebuffers(1, &atlasBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, atlasBuffer);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glViewport(0,0,4096,4096);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /* Create atlas texture */
    glGenTextures(1, &uniTexture);
    glBindTexture(GL_TEXTURE_2D, uniTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 4096, 4096, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    /* Configure frame buffer */
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uniTexture, 0);
    
    //font height
    fontHeight = TTF_FontHeight(uniFont);
}

TrueTypeFont::TrueTypeFont(const char* path, SDL_Color color, ShaderProgram* prog) :
TrueTypeFont(path, color, 32, prog)
{
}

TrueTypeFont::TrueTypeFont()
{
    initFont("C:/Windows/Fonts/comic.ttf");
}

TrueTypeFont::~TrueTypeFont()
{
    glDeleteTextures(1, &ftex);
}

void TrueTypeFont::initFont(const char* fontPath)
{
    std::ifstream file;
    try {
        file.open(fontPath, std::ios::in | std::ios::binary | std::ios::ate);
    }
    catch (std::exception e)
    {
        throw e;
    }
    
    if (file.is_open())
    {
        int size = file.tellg();
        ttf_buffer = new unsigned char[size];
        file.seekg(0, std::ios::beg);
        file.read((char*)ttf_buffer, size);
        
        //fread(ttf_buffer, 1, size, fopen("c:/windows/fonts/Arial.ttf", "rb"));
        file.close();
    }
    
    
    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
    stbtt_BakeFontBitmap(ttf_buffer, 0, 48, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
    
    // can free ttf_buffer at this point
    delete[] ttf_buffer;
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    // can free temp_bitmap at this point
    //delete[] temp_bitmap;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    //collision
    //map data
    //talk to people, dialogue shows up, dialogue fade in (store index or some shit in the vertex buffer), use this
    // to determine what to show/not show
    //
    //puzzles
    
    /*stbtt_aligned_quad q;
    float x, y;
    x = 0;
    y = 0;
    int pos = 'a' - 32;
    stbtt_GetBakedQuad(cdata, 512, 512, pos, &x, &y, &q, 1);`

    std::cout << q.s0 << std::endl;*/
}

void TrueTypeFont::initUnicodeFont(const char* fontPath, int fontPoint, SDL_Color color)
{
    uniFont = TTF_OpenFont(fontPath, fontPoint);
    this->color = color;
}

void TrueTypeFont::getTexCoords(char c, float* x, float* y, glm::vec2* coords, glm::vec2* vecs)
{
    stbtt_aligned_quad q;
    int pos = c - 32;
    
    stbtt_GetBakedQuad(cdata, 512, 512, pos, x, y, &q, 1);
    
    coords[0] = glm::vec2(q.s0, q.t0);
    coords[1] = glm::vec2(q.s0, q.t1);
    coords[2] = glm::vec2(q.s1, q.t1);
    coords[3] = glm::vec2(q.s1, q.t0);
    
    //glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
    //glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
    //glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
    //glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);
    
    //vecs.x = q.x1 - q.x0;
    //vecs.y = q.y1 - q.y0;
    
    vecs[0] = glm::vec2(q.x0, q.y0);
    vecs[1] = glm::vec2(q.x0, q.y1);
    vecs[2] = glm::vec2(q.x1, q.y1);
    vecs[3] = glm::vec2(q.x1, q.y0);
}

GlyphData TrueTypeFont::getGlyphInfo(Uint16 glyph)
{
    auto gIt = glyphCache.find(glyph);
    
    if(gIt != glyphCache.end()) return gIt->second;
    
    SDL_Surface* surfGTest = TTF_RenderGlyph_Blended(uniFont, glyph, color);
    GlyphData glyphDat;
    
    if(!surfGTest)
    {
        glyphDat.adv = -10;
        return glyphDat;
    }
    
    unsigned char* data = static_cast<unsigned char*>(surfGTest->pixels);
    GLuint glyphTexture;
    glGenTextures(1, &glyphTexture);
    glBindTexture(GL_TEXTURE_2D, glyphTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    int fontHeight = TTF_FontHeight(uniFont);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surfGTest->w, surfGTest->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        TTF_GlyphMetrics(uniFont, glyph, &glyphDat.minX, &glyphDat.maxX, &glyphDat.minY, &glyphDat.maxY, &glyphDat.adv);
        
        float asc = TTF_FontAscent(uniFont);
        float des = -TTF_FontDescent(uniFont);
        //std::cout << "Des: " << des << "\n";
        
        glyphDat.h = surfGTest->h;
        glyphDat.w = surfGTest->w;
        
        //std::cout << "point " << fontPoint << " h " << surfGTest->h << "\n";
        
        /*std::cout << "min x " << glyphDat.minX << " max x " << glyphDat.maxX << "\n";
        std::cout << "min y " << glyphDat.minY << " max y " << glyphDat.maxY << "\n";
        std::cout << "w " << glyphDat.maxX - glyphDat.minX << "\n";
        std::cout << "adv " << glyphDat.adv << "\n";*/
        
        if(x + glyphDat.w > 4096)
        {
            x = 0;
            y++;
        }
        
        glyphDat.posX = x;
        glyphDat.posY = y*fontHeight;
        
        glyphDat.uvxMin = (glyphDat.posX) / 4096.0f;
        glyphDat.uvyMin = (glyphDat.posY) / 4096.0f;
        glyphDat.uvxMax = (glyphDat.posX + surfGTest->w) / 4096.0f;
        glyphDat.uvyMax = (glyphDat.posY + surfGTest->h) / 4096.0f;
        
        glyphCache[glyph] = glyphDat;
    }
    else
    {
        std::cout << "Failed to create glyph texture" << glyph << "\n";
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, atlasBuffer);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glViewport(0,0,4096,4096);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uniTexture, 0);
    
    ScreenQuad glyphQuad(glm::vec2(x,y*fontHeight), glm::vec2(surfGTest->w, surfGTest->h), 4096, 4096);
    glyphQuad.textures = { glyphTexture };
    
    //ShaderProgram& glyphProg = assMan->shaderPrograms[15];
    glyphProg->use();
    glyphQuad.draw(*glyphProg);
    
    glDeleteTextures(1, &glyphTexture);
    SDL_FreeSurface(surfGTest);
    
    x = glyphDat.posX + glyphDat.w + 1;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return glyphDat;
}

float TrueTypeFont::getCharLen(char c)
{
    int pos = c - 32;
    stbtt_bakedchar* bakedChar = cdata + pos;
    return bakedChar->xadvance;
}
