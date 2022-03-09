#include "FontRenderer.h"

#include <iostream>

FontRenderer::FontRenderer(const char* text, TrueTypeFont* font, float xPos, float yPos, int lineLength, int lineHeight, int screenWidth, int screenHeight)
{
    texture = font->ftex;
    uniTexture = font->uniTexture;
    
    this->font = font;
    this->xPos = xPos;
    this->yPos = yPos;
    this->lineLength = lineLength;
    this->lineHeight = lineHeight;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    
    createBuffers();
    //setText(text);
}

void FontRenderer::createBuffers()
{
    glGenBuffers(1, &vbo); //create buffer
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
}

void FontRenderer::setupBuffers()
{
    //if (verts.empty() || indices.empty()) return;

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //bind buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * verts.size(), verts.data(), GL_DYNAMIC_DRAW); //load buffer
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_DRAW); //are the vertex and element buffer the same?
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (void*)0); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (void*)(offsetof(FontVertex, uv))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (void*)(offsetof(FontVertex, ind))); //location for vert attrib, size, data type, normalzied?, size of each entry, offset of entries
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0); //unbind vertex array
}

void FontRenderer::convertScreenPosToUV(glm::vec2& pos, int screenWidth, int screenHeight)
{
    pos.x /= screenWidth;
    pos.y /= screenHeight;
    
    pos.x = -1 + (pos.x * 2.0);
    pos.y = -1 + (pos.y * 2.0);
}

void FontRenderer::convertFloatToUV(float& pos, int size)
{
    pos /= size;
    pos = -1 + (pos * 2.0);
}

void FontRenderer::draw(ShaderProgram& program)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uniTexture);
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void FontRenderer::setText(const char* text)
{
    int indexOff = 0;
    float vertIndex = 0;
    float xpos = xPos;
    float ypos = yPos;
    
    float oriXPos = xPos;
    float lineLimit = oriXPos + lineLength;
    
    verts.clear();
    indices.clear();
    
    for (int x = 0; x < strlen(text); x++)
    {
        GLuint i[6] = { 0, 1, 2, 0, 2, 3 };
        
        glm::vec2 coords[4];
        glm::vec2 vecs[4];
        
        float linelen = xpos;
        for (int y = x; text[y] != ' ' && text[y] != '\0'; y++)
        {
            linelen += font->getCharLen(text[y]);
            if (linelen > lineLimit)
            {
                xpos = oriXPos;
                ypos -= lineHeight;
                break;
            }
        }
        
        font->getTexCoords(text[x], &xpos, &ypos, coords, vecs);
        
        FontVertex v[4];
        v[0].vertex = vecs[0];
        convertScreenPosToUV(v[0].vertex, screenWidth, screenHeight);
        v[0].uv = coords[0];
        v[0].ind = vertIndex;
        
        v[1].vertex = glm::vec2(v[0].vertex.x, vecs[1].y);
        convertFloatToUV(v[1].vertex.y, screenHeight);
        v[1].uv = coords[1];
        v[1].ind = vertIndex;
        
        v[2].vertex = glm::vec2(vecs[2].x, v[1].vertex.y);
        convertFloatToUV(v[2].vertex.x, screenWidth);
        v[2].uv = coords[2];
        v[2].ind = vertIndex + 1;
        
        v[3].vertex = glm::vec2(v[2].vertex.x, v[0].vertex.y);
        v[3].uv = coords[3];
        v[3].ind = vertIndex + 1;
        
        for (int y = 0; y < 4; y++) verts.push_back(v[y]);
        for (int y = 0; y < 6; y++) indices.push_back(i[y] + indexOff);
        
        indexOff += 4;
        vertIndex++;
    }
    
    setupBuffers();
}

void FontRenderer::setTextUtf16(const Uint16* text, int len, bool flipBytes, float scale)
{
    int indexOff = 0;
    float vertIndex = 0;
    float xpos = xPos;
    float ypos = yPos;
    
    float oriXPos = xPos;
    float lineLimit = oriXPos + lineLength;
    
    verts.clear();
    indices.clear();
    
    for(int x=0; x<len; x++)
    {
        Uint16 glyph = text[x];
        if(flipBytes) glyph = (glyph>>8) | (glyph<<8);
        
        if(glyph == 65279) continue;
        //if(glyph == 0) break;
        GlyphData glyphDat = font->getGlyphInfo(glyph);
        if(glyphDat.adv < 0) continue;
        
        GLuint i[6] = { 0, 1, 2, 0, 2, 3 };
        
        glm::vec2 coords[4];
        glm::vec2 vecs[4];
        
        float linelen = xpos;
        
        auto getGlyph = [](Uint16 g)->Uint16{
            return (g>>8) | (g<<8);
        };
        
        for (int y = x; getGlyph(text[y]) != 32 && getGlyph(text[y]) != 0 && getGlyph(text[y]) != 65279; y++)
        {
            Uint16 glyph1 = text[y];
            glyph1 = (glyph1>>8) | (glyph1<<8);
            
            GlyphData glyphDat1 = font->getGlyphInfo(glyph1);
            
            linelen += glyphDat1.adv;
            if (linelen > lineLimit)
            {
                xpos = oriXPos;
                ypos -= TTF_FontHeight(font->uniFont);
                break;
            }
        }
        
        glm::vec2 curPos = glm::vec2(xpos, ypos);
        //no kerning, just use the character width
        xpos += glyphDat.w;//(glyph == 106) ? glyphDat.adv * 2 * scale : glyphDat.adv * scale;
        //std::cout << "adv " << glyphDat.adv << " is " << text[x] << "\n";
        
        vecs[0] = curPos + glm::vec2(0, -glyphDat.h * scale);
        vecs[1] = curPos + glm::vec2(0, 0);
        vecs[2] = curPos + glm::vec2(glyphDat.w * scale, 0);
        vecs[3] = curPos  + glm::vec2(glyphDat.w * scale, -glyphDat.h * scale);
        
        coords[0] = glm::vec2(glyphDat.uvxMin, glyphDat.uvyMin);
        coords[1] = glm::vec2(glyphDat.uvxMin, glyphDat.uvyMax);
        coords[2] = glm::vec2(glyphDat.uvxMax, glyphDat.uvyMax);
        coords[3] = glm::vec2(glyphDat.uvxMax, glyphDat.uvyMin);
        
        FontVertex v[4];
        v[0].vertex = vecs[0];
        convertScreenPosToUV(v[0].vertex, screenWidth, screenHeight);
        v[0].uv = coords[0];
        v[0].ind = vertIndex;
        
        v[1].vertex = glm::vec2(v[0].vertex.x, vecs[1].y);
        convertFloatToUV(v[1].vertex.y, screenHeight);
        v[1].uv = coords[1];
        v[1].ind = vertIndex;
        
        v[2].vertex = glm::vec2(vecs[2].x, v[1].vertex.y);
        convertFloatToUV(v[2].vertex.x, screenWidth);
        v[2].uv = coords[2];
        v[2].ind = vertIndex + 1;
        
        v[3].vertex = glm::vec2(v[2].vertex.x, v[0].vertex.y);
        v[3].uv = coords[3];
        v[3].ind = vertIndex + 1;
        
        for (int y = 0; y < 4; y++) verts.push_back(v[y]);
        for (int y = 0; y < 6; y++) indices.push_back(i[y] + indexOff);
        
        indexOff += 4;
        vertIndex++;
    }
    
    setupBuffers();
}

void FontRenderer::setTextUtf16(const Uint16* text, int len)
{
    setTextUtf16(text, len, true, 1.0f);
}

GLuint FontRenderer::createUtf8Image(const char* text, int* w, int* h)
{
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font->uniFont, text, font->color);
    unsigned char* data = static_cast<unsigned char*>(surf->pixels);
    
    GLuint image;
    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    *w = surf->w;
    *h = surf->h;
    
    return image;
}