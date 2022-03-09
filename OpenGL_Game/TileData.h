#pragma once

#include <vector>
#include <string>

struct TileData
{
	unsigned char id = 0;
	unsigned char infoInd = 0;
};

struct PortalData
{
	char* destinationMap;
	glm::vec2 playerLoc = glm::vec2(0);
	bool isPortal = false;
};

struct DynamicData
{
	TileData tileData{};
	unsigned int offsetIndex = 0;
	float oldPos = 0.0f;
	bool isMoving = false;
};

struct DoorData
{
	std::vector<std::pair<size_t, unsigned char>> requirements;
	size_t position = 0;
	bool isOpen = false;
	unsigned char openId = 65;
	unsigned char closedId = 66;
    unsigned char dir[3] = { 0, 0, 0 };
};

struct TileEventData
{
	int id = 0;
	bool active = false;
	bool limited = false;
};

struct TileInteractData
{
    size_t position = 0;
    int type = -1;
    bool active = false;
    bool interacting = false;
    
    union
    {
        //Type 0
        struct ObjectType
        {
            char name[64];
            char lines[1024]; //Dialogue lines seperated by new lines
        } object;
        
        //Type 1
        struct JournalType
        {
            char name[64];
            char lines[1024]; //Dialogue lines seperated by new lines
        } journal;
    };
    
};