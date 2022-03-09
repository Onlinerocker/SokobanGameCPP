#pragma once
#include <GL\glew.h>

namespace TextureUtil
{
	/* Have offsets for each asset here */
    
	GLuint createTexture(const char* path, bool flip, GLint filter);
	GLuint createTexture(const char* path, bool flip, GLint filter, int* width, int* height);
	void createTexture(const char* path, bool flip, GLint filter, int* width, int* height, GLuint* ids, unsigned int total);
    
	void getTexels(unsigned char* dest, const unsigned char* blob, int blobWidth, int blobHeight, int texelWidth, int texelHeight, int x, int y);
	char* loadBlob(const char* path);
	void createBlob(const char* outPath, const char* inPath);
	GLuint createTextureFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter);
	GLuint createUITextureArrayFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter);
	GLuint createUIMenuTextureArrayFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter);
    GLuint createTilesTextureArrayFromBlob(char* blob, size_t* index, size_t* size, int* width, int* height, bool flip, GLint filter);
};



