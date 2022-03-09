#include "TextureUtil.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include <iostream>
#include <fstream>

GLuint TextureUtil::createTexture(const char* path, bool flip, GLint filter)
{
    int width, height, nrChans;
    //stbi_set_flip_vertically_on_load(true);
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path, &width, &height, &nrChans, STBI_rgb_alpha);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //TODO make configurable
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture " << path << "\n";
    }
    
    stbi_image_free(data);
    
    return texture;
}

GLuint TextureUtil::createTexture(const char* path, bool flip, GLint filter, int* width, int* height)
{
    int nrChans;
    //stbi_set_flip_vertically_on_load(true);
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path, width, height, &nrChans, STBI_rgb_alpha);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //TODO make configurable
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded " << path << "\n";
    }
    else
    {
        std::cout << "Failed to load texture " << path << "\n";
    }
    
    stbi_image_free(data);
    
    return texture;
}

void TextureUtil::createTexture(const char* path, bool flip, GLint filter, int* width, int* height, GLuint* ids, unsigned int total)
{
    int nrChans;
    //stbi_set_flip_vertically_on_load(true);
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(path, width, height, &nrChans, STBI_rgb_alpha);
    
    for (unsigned int i = 0; i < total; i++)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //TODO make configurable
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture " << path << "\n";
        }
        
        ids[i] = texture;
    }
    
    stbi_image_free(data);
    
}

char* TextureUtil::loadBlob(const char* path)
{
    std::ifstream istream(path, std::ios::in | std::ios::binary | std::ios::ate);
    
    if (istream.is_open())
    {
        std::streampos size = istream.tellg();
        char* dest = new char[size];
        istream.seekg(0, std::ios::beg);
        istream.read(dest, size);
        istream.close();
        
        //std::cout << "Loaded blob size " << size << "\n";
        return dest;
    }
    
    return nullptr;
}

void TextureUtil::createBlob(const char* outPath, const char* inPath)
{
    std::ofstream ostream(outPath, std::ios::out | std::ios::binary | std::ios::app | std::ios::ate);
    
    if (ostream.is_open())
    {
        int width = 0;
        int height = 0;
        int nrChans = 0;
        
        unsigned char* data = stbi_load(inPath, &width, &height, &nrChans, STBI_rgb_alpha);
        
        size_t size = width * height * 4;
        
        ostream.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        ostream.write(reinterpret_cast<const char*>(&width), sizeof(int));
        ostream.write(reinterpret_cast<const char*>(&height), sizeof(int));
        ostream.write(reinterpret_cast<const char*>(data), size);
        
        ostream.close();
        stbi_image_free(data);
        
        std::cout << "Output blob: " << inPath << " size: " << size << " bytes\n";
    }
    
}

//change tile texture atlast to use a texture array
GLuint TextureUtil::createTextureFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter)
{
    size = reinterpret_cast<size_t*>(blob + *index);
    int* w = reinterpret_cast<int*>(blob + *index + sizeof(size_t));
    *width = *w;
    int* h = reinterpret_cast<int*>(blob + *index + sizeof(size_t) + sizeof(int));
    *height = *h;
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //TODO make configurable
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    
    size_t indexTemp = *index + sizeof(size_t) + (2 * sizeof(int));
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<unsigned char*>(blob + *index + sizeof(size_t) + (2 * sizeof(int))) );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    *index = indexTemp + *size;
    
    //std::cout << "Created texture from blob (Size: " << *size << " bytes, WH: " << *width << "x" << *h << ")\n";
    return texture;
}

GLuint TextureUtil::createUITextureArrayFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter)
{
    size = reinterpret_cast<size_t*>(blob + *index);
    int* w = reinterpret_cast<int*>(blob + *index + sizeof(size_t));
    *width = *w;
    int* h = reinterpret_cast<int*>(blob + *index + sizeof(size_t) + sizeof(int));
    *height = *h;
    
    unsigned char* texels = reinterpret_cast<unsigned char*>(blob + *index + sizeof(size_t) + (2 * sizeof(int)));
    
    unsigned char resume[250 * 125 * 4];
    getTexels(resume, texels, *width, *height, 250, 125, 0, 0);
    
    unsigned char exit[250 * 125 * 4];
    getTexels(exit, texels, *width, *height, 250, 125, 250, 0);
    
    unsigned char bar[500 * 25 * 4];
    getTexels(bar, texels, *width, *height, 500, 25, 0, 125);
    
    unsigned char knob[500 * 25 * 4];
    getTexels(knob, texels, *width, *height, 50, 25, 0, 150);
    
    unsigned char text[500 * 49 * 4];
    getTexels(text, texels, *width, *height, 500, 49, 0, 175);
    
    unsigned char box[55 * 26 * 4];
    getTexels(box, texels, *width, *height, 55, 26, 0, 224);
    
    unsigned char back[160 * 80 * 4];
    getTexels(back, texels, *width, *height, 160, 80, 500, 0);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);
    
    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_SRGB8_ALPHA8, 250, 125, 2);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, 500, 250,
                 7, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    size_t indexTemp = *index + sizeof(size_t) + (2 * sizeof(int));
    
    /*glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<unsigned char*>(blob + *index + sizeof(size_t) + (2 * sizeof(int))));
    glGenerateMipmap(GL_TEXTURE_2D);*/
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 250, 125, 1, GL_RGBA, GL_UNSIGNED_BYTE, resume);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, 250, 125, 1, GL_RGBA, GL_UNSIGNED_BYTE, exit);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, 500, 25, 1, GL_RGBA, GL_UNSIGNED_BYTE, bar);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, 50, 25, 1, GL_RGBA, GL_UNSIGNED_BYTE, knob);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 4, 500, 49, 1, GL_RGBA, GL_UNSIGNED_BYTE, text);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 5, 55, 26, 1, GL_RGBA, GL_UNSIGNED_BYTE, box);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 6, 160, 80, 1, GL_RGBA, GL_UNSIGNED_BYTE, back);
    
    *index = indexTemp + *size;
    return texture;
}

GLuint TextureUtil::createUIMenuTextureArrayFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter)
{
    size = reinterpret_cast<size_t*>(blob + *index);
    int* w = reinterpret_cast<int*>(blob + *index + sizeof(size_t));
    *width = *w;
    int* h = reinterpret_cast<int*>(blob + *index + sizeof(size_t) + sizeof(int));
    *height = *h;
    
    unsigned char* texels = reinterpret_cast<unsigned char*>(blob + *index + sizeof(size_t) + (2 * sizeof(int)));
    
    unsigned char start[410 * 90 * 4];
    getTexels(start, texels, *width, *height, 410, 90, 0, 0);
    
    unsigned char settings[410 * 90 * 4];
    getTexels(settings, texels, *width, *height, 410, 90, 0, 100);
    
    unsigned char exit[410 * 90 * 4];
    getTexels(exit, texels, *width, *height, 410, 90, 0, 200);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);
    
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, 410, 90,
                 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    size_t indexTemp = *index + sizeof(size_t) + (2 * sizeof(int));
    
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 410, 90, 1, GL_RGBA, GL_UNSIGNED_BYTE, start);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, 410, 90, 1, GL_RGBA, GL_UNSIGNED_BYTE, settings);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, 410, 90, 1, GL_RGBA, GL_UNSIGNED_BYTE, exit);
    
    *index = indexTemp + *size;
    return texture;
}

GLuint TextureUtil::createTilesTextureArrayFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter)
{
    size = reinterpret_cast<size_t*>(blob + *index);
    int* w = reinterpret_cast<int*>(blob + *index + sizeof(size_t));
    *width = *w;
    int* h = reinterpret_cast<int*>(blob + *index + sizeof(size_t) + sizeof(int));
    *height = *h;
    
    unsigned char* texels = reinterpret_cast<unsigned char*>(blob + *index + sizeof(size_t) + (2 * sizeof(int)));
    
    int xTiles = *width / 17;
    int yTiles = *height / 17;
    int idCount = xTiles * yTiles;
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);
    
    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_SRGB8_ALPHA8, 250, 125, 2);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, 17, 17,
                 idCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    size_t indexTemp = *index + sizeof(size_t) + (2 * sizeof(int));
    
    int id = 0;
    unsigned char tileData[17 * 17 * 4];
    for (int y = *height-17; y >= 0; y -= 17)
    {
        for (int x = 0; x < *width; x += 17)
        {
            getTexels(tileData, texels, *width, *height, 17, 17, x, y);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, id, 17, 17, 1, GL_RGBA, GL_UNSIGNED_BYTE, tileData);
            id++;
        }
    }
    
    *index = indexTemp + *size;
    
    return texture;
}

void TextureUtil::getTexels(unsigned char* dest, const unsigned char* blob, int blobWidth, int blobHeight, int texelWidth, int texelHeight, int x, int y)
{
    int di = 0;
    int id = 0;
    
    for (int by = y; by < y + texelHeight; by++)
    {
        for (int bx = x*4; bx < 4*(x + texelWidth); bx+=4)
        {
            int i = (by * blobWidth * 4) + bx;
            //i *= 4;
            dest[di] = blob[i];
            dest[di + 1] = blob[i + 1];
            dest[di + 2] = blob[i + 2];
            dest[di + 3] = blob[i + 3];
            
            /*if (by == y || di <= 0 || di >= 125000)
            {
                dest[di] = 255;
                dest[di + 1] = 0;
                dest[di + 2] = 0;
                dest[di + 3] = blob[i + 3];
            }*/
            
            di += 4;
        }
    }
}
