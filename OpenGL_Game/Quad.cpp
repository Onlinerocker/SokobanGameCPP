#include "Quad.h"
#include "TextureUtil.h"
#include <iostream>

Quad::Quad(const char* imagePath, glm::vec3 location, glm::vec2 size)
{
	this->imagePath = imagePath;
	GLuint tId = TextureUtil::createTexture(imagePath, false, GL_NEAREST, &width, &height);
    
	setupQuad(tId, location, size, glm::vec2(1,1));
}

Quad::Quad(const char* imagePath, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize)
{
	this->imagePath = imagePath;
	GLuint tId = TextureUtil::createTexture(imagePath, false, GL_NEAREST, &width, &height);
    
	setupQuad(tId, location, size, uvSize);
}

Quad::Quad(GLuint texture, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize)
{
	this->imagePath = "";
	setupQuad(texture, location, size, uvSize);
}

Quad::~Quad()
{
	delete mesh;
}

void Quad::setupQuad(GLuint texture, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize)
{
	std::vector<Vertex> verts;
	std::vector<Texture> texts;
	std::vector<GLuint> inds = { 0, 1, 2, 0, 2, 3 };
    
	//verts
	addVert(verts, location, glm::vec2(0, 0));
	addVert(verts, location + glm::vec3(0, size.y, 0), glm::vec2(0, uvSize.y));
	addVert(verts, location + glm::vec3(size.x, size.y, 0), uvSize);
	addVert(verts, location + glm::vec3(size.x, 0, 0), glm::vec2(uvSize.x, 0));
    
	Texture t;
	t.id = texture;
	t.type = diffuse;
    
	texts.push_back(t);
    
    std::cout << "pushing back img " << texture << "\n";
    
	//mesh
	mesh = new Mesh(verts, inds, texts);
}

void Quad::addVert(std::vector<Vertex>& verts, glm::vec3 pos, glm::vec2 uv)
{
	Vertex v;
    
	v.vertex = pos;
	v.uv = uv;
    
	verts.push_back(v);
}

void Quad::draw(ShaderProgram& program)
{
	mesh->draw(program);
}

int Quad::getHeight()
{
	return height;
}

int Quad::getWidth()
{
	return width;
}