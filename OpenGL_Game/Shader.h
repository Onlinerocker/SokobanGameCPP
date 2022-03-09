#pragma once

#include <GL\glew.h>

class Shader
{
    
    private:
	const char* fileName;
	GLenum shaderType;
	GLuint shader;
    
    public:
    Shader();
	Shader(const char* fileName, GLenum shaderType);
	~Shader();
    
    void cleanUp();
    
	GLuint getShader();
    
};

