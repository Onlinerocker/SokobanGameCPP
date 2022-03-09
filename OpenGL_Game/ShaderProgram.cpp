#include "ShaderProgram.h"

//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ShaderProgram::ShaderProgram()
{
	shaderProgram = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
}

GLuint ShaderProgram::getShaderProgram()
{
	return shaderProgram;
}

void ShaderProgram::attachShader(Shader& shader)
{
	glAttachShader(shaderProgram, shader.getShader());
}

void ShaderProgram::linkProgram()
{
	glLinkProgram(shaderProgram);
}

void ShaderProgram::use()
{
	glUseProgram(shaderProgram);
}

void ShaderProgram::cleanUp()
{
	glDeleteProgram(shaderProgram);
}

void ShaderProgram::getAttribute(const char* attribute)
{
	attributes[attribute] = glGetUniformLocation(shaderProgram, attribute);
}

void ShaderProgram::setFloat(const char* attribute, GLfloat value)
{
	glUniform1f(attributes[attribute], value);
}

void ShaderProgram::setInt(const char* attribute, GLint value)
{
	glUniform1i(attributes[attribute], value);
}

void ShaderProgram::setMat4(const char* attribute, const glm::mat4* mat)
{
	glUniformMatrix4fv(attributes[attribute], 1, GL_FALSE, glm::value_ptr(*mat));
}

void ShaderProgram::setVec2(const char* attribute, const glm::vec2 vec)
{
	glUniform2f(attributes[attribute], vec.x, vec.y);
}

void ShaderProgram::setVec3(const char* attribute, const glm::vec3 vec)
{
	glUniform3f(attributes[attribute], vec.x, vec.y, vec.z);
}

void ShaderProgram::setVec4(const char* attribute, const glm::vec4 vec)
{
	glUniform4f(attributes[attribute], vec.x, vec.y, vec.z, vec.w);
}