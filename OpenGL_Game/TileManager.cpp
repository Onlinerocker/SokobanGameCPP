#include "TileManager.h"

#include <iostream>

TileManager::TileManager(unsigned int atlasWidth, unsigned int atlasHeight, unsigned int tileSize)
{  	
	unsigned int width = atlasWidth;
	unsigned int height = atlasHeight;
	unsigned int size = tileSize;
	unsigned char id = 1;
    
	float xTile = static_cast<float>(size) / static_cast<float>(width);
	float yTile = static_cast<float>(size) / static_cast<float>(height);
    
	solids = new bool[255];
	solids[0] = false;
    
	walkableIds = { 1, 5, 37, 38, 39, 40, 65, 60, 71, 93, 119, 138};
	for (float y = 1.0 - yTile; y > -yTile; y -= yTile)
	{
		y = glm::clamp(y, 0.0f, 1.0f);
		for (float x = 0; x < 1.0; x += xTile)
		{		
			uvs[id] = glm::vec4(x, y, x + xTile, y + yTile);
			if (std::find(walkableIds.begin(), walkableIds.end(), id) == walkableIds.end()) solids[id] = true;
			else solids[id] = false;
			id++;
		}
	}
    
	for (unsigned char i = id; i < 255; i++)
	{
		solids[i] = false;
	}
    
    solids[250] = true; //npc tile
    solids[253] = true; //magic laser intersection tile
    solids[254] = true; //other magic laser intersection tile
}

TileManager::~TileManager()
{
	delete[] solids;
}

bool TileManager::isTileSolid(unsigned char id)
{
	return solids[id];
}

glm::vec4 TileManager::getTileUV(unsigned int id)
{
	std::unordered_map<unsigned int, glm::vec4>::iterator it = uvs.find(id);
	if (it != uvs.end()) return it->second;
	else
	{
		std::cout << "[ERR] Could not find tile UV for ID " << id << std::endl;
		return glm::vec4(0);
	}
}