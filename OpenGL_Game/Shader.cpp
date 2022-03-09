#include "Shader.h"
#include "ShaderUtil.h"
#include <iostream>

Shader::Shader()
{
    fileName = "";
    shaderType = GL_VERTEX_SHADER;
    shader = 0;
}

Shader::Shader(const char* fileName, GLenum shaderType)
{
	this->fileName = fileName;
	this->shaderType = shaderType;
    
	GLchar* shaderCode = nullptr;
	GLint len = 0;
	LoadShader(fileName, shaderCode, len);
    
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
    
    GLint success = GL_FALSE;
    char infoLog[512];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        glGetShaderInfoLog(shader, 512, &logSize, infoLog);
        
        std::cout << "[ERR] Shader failed to compile: " << fileName << std::endl;
        std::cout << "::: " << infoLog << std::endl;
    }
    
    delete[] shaderCode;
}

Shader::~Shader()
{
    //cleanUp();
}

GLuint Shader::getShader()
{
	return shader;
}

void Shader::cleanUp()
{
    glDeleteShader(shader);
}
