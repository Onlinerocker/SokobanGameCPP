#pragma once

#include <GL\glew.h>
#include <map>

#include <glm/glm.hpp>

#include "Shader.h"

class ShaderProgram
{
    
    private:
	GLuint shaderProgram;
	std::map<const char*, GLuint> attributes;
    
    public:
	ShaderProgram();
	~ShaderProgram();
    
	GLuint getShaderProgram();
    
	void attachShader(Shader& shader);
	void linkProgram();
	void use();
    void cleanUp();
    
	void getAttribute(const char* attribute);
	void setFloat(const char* attribute, GLfloat value);
	void setInt(const char* attribute, GLint value);
	void setMat4(const char* attribute, const glm::mat4* mat);
	void setVec2(const char* attribute, const glm::vec2 vec);
	void setVec3(const char* attribute, const glm::vec3 vec);
    void setVec4(const char* attribute, const glm::vec4 vec);
    
};

