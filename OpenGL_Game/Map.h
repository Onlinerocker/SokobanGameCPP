#pragma once

#define DEBUG_BUILD

#include "glm/glm.hpp"
#include "TileManager.h"
#include "QuadAnimated.h"
#include "TileData.h"
#include "WorldEnums.h"

struct StateStamp
{
	glm::vec2 playerFrom;
	WorldEnums::WRL_DIR directionFrom;
	DynamicData dynamicData[4096];
	TileData staticData1[4096];
    bool claimedData[4096];
	std::vector<glm::vec2> dynamicOffsets;
};

class Map
{
    
    private:
	unsigned int size;
    
    public:
	std::string mapFile;
	std::vector<DoorData> doors;
#ifdef DEBUG_BUILD
	bool justLoaded;
    bool walkThrough;
#endif
	bool isSnowing;
	bool isCave;
	bool resettable;
	float tileSize;
	int width, height;
	glm::vec3 pos;
	glm::vec3 worldPos;
	glm::vec2 resetPos;
	std::vector<TileInteractData> interactDataVec;
    std::vector<glm::vec2> dynamicOffsets;
	std::vector<glm::vec4> dynamicUvs;
	std::vector<float> dynamicGlowing;
	TileManager* tileManager;
	TileData* staticData;
	TileData* staticData1;
	TileData* staticTopData;
	DynamicData* dynamicData;
	PortalData* portalData;
	TileEventData* eventData;
    glm::vec3* staticDataColors;
    glm::vec3* staticData1Colors;
	bool* claimedData;
	StateStamp stateStamps[1028];
	int stampNum;
	int stampLast;
	int stampBtm;
    
	Map(TileManager* tileManager, glm::vec3 pos, float tileSize, int width, int height); //allocate data for the static and dynamic data
	~Map(); //deallocate data for static and dynamic data
    
	void setTileStatic(unsigned int x, unsigned int y, unsigned char id, int layer);
	void setTileStaticTop(unsigned int x, unsigned int y, unsigned char id);
	void setTileDynamic(unsigned int x, unsigned int y, unsigned char id);
	void setClaimTileStatic(unsigned int x, unsigned int y, bool claim);
	void setTileEvent(unsigned int x, unsigned int y, size_t id, bool limited, bool active);
    
	TileData getTileStatic(unsigned int x, unsigned int y, int layer) const;
	TileData getTileStaticTop(unsigned int x, unsigned int y) const;
	TileData getTileStaticWorldPos(int x, int y) const;
	DynamicData getTileDynamic(unsigned int x, unsigned int y) const;
	DynamicData getTileDynamicWorldPos(int x, int y) const;
	TileEventData* getTileEventDataPtrWorldPos(int x, int y) const;
    const TileInteractData* getTileInteractDataWorldPos(int x, int y) const;
    
	bool canWalkAtWorldPos(int x, int y) const;
	bool canMoveIntoWorldPos(glm::vec2 cur, glm::vec2 dest) const;
    
	PortalData getPortalAtWorldPos(int x, int y, bool* hasPortal) const;
	void setPortalAtWorldPos(int x, int y, int dx, int dy, const char* map);
	void deletePortalAtWorldPos(int x, int y);
	
	void setWidthHeight(int width, int height);
	void setStaticData(TileData* data);
	void setStaticTopData(TileData* data);
    
	bool saveMapToFile(const char* path);
	bool loadMapFromFile(const char* path);
    
	const char* getMapFileName();
};

