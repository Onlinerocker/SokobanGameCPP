#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include <unordered_map>

class TileManager
{
private:
	std::unordered_map<unsigned int, glm::vec4> uvs; //UV coordinates of each ID in the texture atlas

public:
	std::vector<unsigned char> walkableIds;
	bool* solids; //Tiles you cannot walk through

	TileManager(unsigned int atlasWidth, unsigned int atlasHeight, unsigned int tileSize);
	~TileManager();

	bool isTileSolid(unsigned char id);
	glm::vec4 getTileUV(unsigned int id);
};

