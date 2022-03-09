#pragma once

#include "Mesh.h"

class Quad
{
private:
	Mesh* mesh;
	const char* imagePath;

	int width, height;

	void setupQuad(GLuint texture, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize);
	void addVert(std::vector<Vertex>& verts, glm::vec3 pos, glm::vec2 uv);

public:
	Quad(const char* imagePath, glm::vec3 location, glm::vec2 size);
	Quad(const char* imagePath, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize);
	Quad(GLuint texture, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize);
	~Quad();

	void draw(ShaderProgram& program);

	int getWidth();
	int getHeight();

};

