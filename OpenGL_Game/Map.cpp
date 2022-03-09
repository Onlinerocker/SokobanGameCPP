#include "Map.h"

#include <iostream>
#include <exception>
#include <fstream>

Map::Map(TileManager* tileManager, glm::vec3 pos, float tileSize, int width, int height)
{
#ifdef DEBUG_BUILD
	this->justLoaded = false;
    this->walkThrough = false;
#endif
    
	this->width = width;
	this->height = height;
	this->size = width * height;
	this->tileSize = tileSize;
	this->pos = pos * tileSize;
	this->worldPos = pos;
	this->tileManager = tileManager;
	this->mapFile = "";
	this->isSnowing = true;
	this->isCave = false;
	this->resettable = false;
    
	unsigned long size = width * height;
	staticData = new TileData[size];
	staticData1 = new TileData[size];
	staticTopData = new TileData[size];
	dynamicData = new DynamicData[size];
	portalData = new PortalData[size];
	claimedData = new bool[size];
	eventData = new TileEventData[size];
	resetPos = glm::vec2(0);
    staticData1Colors = new glm::vec3[size];
    staticDataColors = new glm::vec3[size];
    
    for(int i=0; i < size; i++)
    {
        staticData1Colors[i] = glm::vec3(1);
    }
    
    for(int i=0; i < size; i++)
    {
        staticDataColors[i] = glm::vec3(1);
    }
    
	for (unsigned long x = 0; x < size; x++) 
	{
		TileData d{};
		PortalData pd{};
		DynamicData dd{};
		TileEventData ed{};
        
		staticData[x] = d;
		staticData1[x] = d;
		staticTopData[x] = d;
        
		portalData[x] = pd;
        
		dynamicData[x] = dd;
		dynamicData[x].tileData = d;
		claimedData[x] = false;
		eventData[x] = ed;
	}
    
	stampNum = 0;
	stampLast = 0;
	stampBtm = 0;
}

Map::~Map()
{
	for (unsigned int x = 0; x < size; x++)
	{
		delete[] portalData[x].destinationMap;
	}
    
	delete[] portalData;
	delete[] staticData;
	delete[] dynamicData;
	delete[] staticTopData;
}

void Map::setTileStatic(unsigned int x, unsigned int y, unsigned char id, int layer)
{
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] static: Could not find local position in map: " << x << "," << y << std::endl;
		return;
	}
    
	unsigned long index = (y * width) + x;
    
	TileData d;
	d.id = id;
	d.infoInd = 0;
    
	layer == 0 ? staticData[index] = d : staticData1[index] = d;
}

void Map::setTileStaticTop(unsigned int x, unsigned int y, unsigned char id)
{
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] static: Could not find local position in map: " << x << "," << y << std::endl;
		return;
	}
    
	unsigned long index = (y * width) + x;
    
	TileData d;
	d.id = id;
	d.infoInd = 0;
    
	staticTopData[index] = d;
}

void Map::setTileDynamic(unsigned int x, unsigned int y, unsigned char id)
{
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] static: Could not find local position in map: " << x << "," << y << std::endl;
		return;
	}
    
	unsigned long index = (y * width) + x;
    
	DynamicData* d = &dynamicData[index];
    
	if (d->tileData.id > 0)
	{
		unsigned int i = d->offsetIndex;
        
		if (id == 0)
		{
			dynamicOffsets.erase(dynamicOffsets.begin() + i);
			dynamicUvs.erase(dynamicUvs.begin() + i);
			dynamicGlowing.erase(dynamicGlowing.begin() + i);
            
			for (unsigned int x = i; x < dynamicOffsets.size(); x++)
			{
				glm::vec2 *cur = &dynamicOffsets[x];
				unsigned int pos = cur->y * width + cur->x;
				dynamicData[pos].offsetIndex -= 1;
			}
		}
		else if (id > 0 && d->tileData.id != id)
		{
			glm::vec4 uv = tileManager->getTileUV(id);
			dynamicUvs[i] = uv;
		}
        
		d->tileData.id = id;
	}
	else if(d->tileData.id == 0 && id > 0)
	{
		//glm::vec4 uv = tileManager->getTileUV(id);
        
		dynamicOffsets.push_back(glm::vec2(x, y));
		dynamicUvs.push_back(glm::vec4(0,0,0,id));
		dynamicGlowing.push_back(0.0f);
        
		d->tileData.id = id;
		d->offsetIndex = dynamicOffsets.size() - 1;
	}
	else
	{
		d->tileData.id = 0;
		d->offsetIndex = 0;
	}
    
}

void Map::setClaimTileStatic(unsigned int x, unsigned int y, bool claim)
{
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] static: Could not find local position in map: " << x << "," << y << std::endl;
		return;
	}
    
	unsigned long index = (y * width) + x;
	claimedData[index] = claim;
}

void Map::setTileEvent(unsigned int x, unsigned int y, size_t id, bool limited, bool active)
{
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] static: Could not find local position in map: " << x << "," << y << std::endl;
		return;
	}
    
	unsigned long index = (y * width) + x;
	TileEventData& e = eventData[index];
	e.id = id;
	e.active = active;
	e.limited = limited;
}

TileData Map::getTileStatic(unsigned int x, unsigned int y, int layer) const
{
	TileData t;
	t.id = 0;
	t.infoInd = 0;
    
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] static: Could not find local position in map: " << x << "," << y << std::endl;
		return t;
	}
    
	unsigned long index = (y * width) + x;
	return layer == 0 ? staticData[index] : staticData1[index];
}

TileData Map::getTileStaticTop(unsigned int x, unsigned int y) const
{
	TileData t;
	t.id = 0;
	t.infoInd = 0;
    
	if (x >= width || y >= height)
	{
		std::cout << "[ERROR] staticTop: Could not find local position in map: " << x << "," << y << std::endl;
		return t;
	}
    
	unsigned long index = (y * width) + x;
	return staticTopData[index];
}


DynamicData Map::getTileDynamic(unsigned int x, unsigned int y) const
{
	DynamicData d{};
    
	if (x >= 0 && y >= 0 && (x >= width || y >= height))
	{
		std::cout << "[ERROR] dynamic: Could not find local position in map: " << x << "," << y << std::endl;
		return d;
	}
    
	unsigned long index = (y * width) + x;
	return dynamicData[index];
}

DynamicData Map::getTileDynamicWorldPos(int x, int y) const
{
	DynamicData d{};
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
    
	if (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height)
	{
		std::cout << "[ERROR] dynamic: Could not find world position in map: " << p.x << "," << p.y << std::endl;
		return d;
	}
    
	unsigned long index = (p.y * width) + p.x;
	return dynamicData[index];
}

TileData Map::getTileStaticWorldPos(int x, int y) const
{
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
	
	TileData t;
	t.id = 0;
	t.infoInd = 0;
    
	if (p.x >= width || p.x < 0 || p.y >= height || p.y < 0)
	{
		std::cout << "[ERROR] Could not find world position in map: " << x << "," << y << std::endl;
		return t;
	}
    
	return getTileStatic(p.x, p.y, 0);
    
}

TileEventData* Map::getTileEventDataPtrWorldPos(int x, int y) const
{
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
	
	if (p.x >= width || p.x < 0 || p.y >= height || p.y < 0)
	{
		std::cout << "[ERROR] Could not find world position in map: " << x << "," << y << std::endl;
		return nullptr;
	}
    
	size_t index = (p.y * width) + p.x;
	return &eventData[index];
}

const TileInteractData* Map::getTileInteractDataWorldPos(int x, int y) const
{
    int xLoc = x - worldPos.x;
    int yLoc = y - worldPos.y;
    
    for(int i=0; i < interactDataVec.size(); i++)
    {
        if(interactDataVec[i].position == (yLoc * width) + xLoc)
        {
            return &interactDataVec[i];
        }
    }
    
    return nullptr;
}

//RtlCaptureStackBackTrace
//SymFromAddr
//SymGetLineFromAddr64

bool Map::canWalkAtWorldPos(int x, int y) const
{
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
	TileData d = getTileStaticWorldPos(x, y);
    
	size_t i = p.y * width + p.x;
    
	bool claimed = claimedData[i];
	bool inBounds = (p.x >= 0 && p.y >= 0 && p.x < width&& p.y < height);
    
	TileData d1 = staticData1[i];
	return !tileManager->isTileSolid(d.id) && (d1.id == 0 || !tileManager->isTileSolid(d1.id)) && inBounds && d.id > 0 && !claimed;
}

bool Map::canMoveIntoWorldPos(glm::vec2 cur, glm::vec2 dest) const
{
#ifdef DEBUG_BUILD
    if(walkThrough) return true;
#endif
	bool canWalk = canWalkAtWorldPos(dest.x, dest.y);
	DynamicData dd = getTileDynamicWorldPos(dest.x, dest.y);
    
	glm::vec2 dir = dest - cur;
	dir = glm::normalize(dir);
    
	glm::vec2 destDir = dest + dir;
    
	DynamicData ddDir = getTileDynamicWorldPos(destDir.x, destDir.y);
    
	if (dd.tileData.id > 0 && ddDir.tileData.id <= 0)
	{
		canWalk = canWalk && (canWalkAtWorldPos(destDir.x, destDir.y));
	}
	else if (dd.tileData.id >= 59 && ddDir.tileData.id > 0)
	{
		DynamicData ddDir1 = getTileDynamicWorldPos(destDir.x + dir.x, destDir.y + dir.y);
		canWalk = canWalk && (canWalkAtWorldPos(destDir.x + dir.x, destDir.y + dir.y)) && ddDir1.tileData.id <= 0;
	}
	else if (dd.tileData.id > 0 && dd.tileData.id < 59 && ddDir.tileData.id > 0)
	{
		canWalk = false;
	}
    
	return canWalk;
}

PortalData Map::getPortalAtWorldPos(int x, int y, bool* hasPortal) const
{
	PortalData d{};
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
    
	unsigned int portalInd = p.y * width + p.x;
	if (portalInd >= size)
	{
		*hasPortal = false;
		std::cout << "[ERROR] Could not get portal in map: " << x << "," << y << " s " << size << std::endl;
		return d;
	}
    
	d = portalData[portalInd];
    
	*hasPortal = d.isPortal;
    
	return d;
}

void Map::setPortalAtWorldPos(int x, int y, int dx, int dy, const char* map)
{
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
    
	if ((p.x >= width || p.x < 0) || (p.y >= height || p.y < 0))
	{
		std::cout << "[ERROR] Could not set portal in map: " << x << "," << y << std::endl;
		return;
	}
    
	unsigned int portalInd = p.y * width + p.x;
	portalData[portalInd].destinationMap = new char[128];
	strcpy(portalData[portalInd].destinationMap, map);
	portalData[portalInd].isPortal = true;
	portalData[portalInd].playerLoc.x = dx;
	portalData[portalInd].playerLoc.y = dy;
    
	//std::cout << portalData[portalInd].playerLoc.x << " " << portalData[portalInd].playerLoc.y << std::endl;
}

void Map::deletePortalAtWorldPos(int x, int y)
{
	glm::vec2 p(x - worldPos.x, y - worldPos.y);
    
	unsigned int portalInd = p.y * width + p.x;
    
	//if (portalData[portalInd].destinationMap) delete[] portalData[portalInd].destinationMap;
	portalData[portalInd].isPortal = false;
	portalData[portalInd].playerLoc = glm::vec2(0);
}

void Map::setWidthHeight(int width, int height)
{
	unsigned int oldHeight = this->height;
	this->height = height;
    
	unsigned int oldWidth = this->width;
	this->width = width;
	this->size = width * height;
    
	std::vector<DoorData> newDoors;
	for (int i = 0; i < doors.size(); i++)
	{
		DoorData& d = doors[i];
		int y = d.position / oldWidth;
		int x = d.position % oldWidth;
        
		if (x >= width || y >= height)
		{
			doors.erase(doors.begin() + i);
			i--;
		}
		else
		{
			d.position = y * width + x;
		}
	}
    
	unsigned int newSize = width * height;
	TileData* newStaticData = new TileData[newSize];
	TileData* newStaticTopData = new TileData[newSize];
	PortalData* newPortalData = new PortalData[newSize];
	DynamicData* newDynamicData = new DynamicData[newSize];
	TileEventData* newEventData = new TileEventData[newSize];
	
	delete[] claimedData;
	claimedData = new bool[newSize];
    
	dynamicOffsets.clear();
	dynamicUvs.clear();
	dynamicGlowing.clear();
    
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			unsigned int newIndex = (y * width) + x;
			if (y < oldHeight && x < oldWidth)
			{
				unsigned int index = (y * oldWidth) + x;
                
				newStaticData[newIndex] = staticData[index];
				newStaticTopData[newIndex] = staticTopData[index];
				newPortalData[newIndex] = portalData[index];
				newDynamicData[newIndex] = dynamicData[index];
				newEventData[newIndex] = eventData[index];
                
				//push offset
				//push uv
				//update offset index
				unsigned char id = newDynamicData[newIndex].tileData.id;
				if (id > 0)
				{
					dynamicOffsets.push_back(glm::vec2(x, y));
					dynamicUvs.push_back(glm::vec4(0, 0, 0, id));
					dynamicGlowing.push_back(0.0f);
					newDynamicData[newIndex].offsetIndex = dynamicOffsets.size() - 1;
				}
			}
			else
			{
				TileData d;
				d.id = 1;
				d.infoInd = 0;
                
				newStaticData[newIndex] = d;
                
				d.id = 0;
				newStaticTopData[newIndex] = d;
				newPortalData[newIndex] = PortalData{};
				newEventData[newIndex] = TileEventData{};
			}
            
			claimedData[newIndex] = false;
		}
	}
    
	delete[] staticData;
	delete[] staticTopData;
	delete[] portalData;
	delete[] dynamicData;
	delete[] eventData;
    
	staticData = newStaticData;
	staticTopData = newStaticTopData;
	portalData = newPortalData;
	dynamicData = newDynamicData;
	eventData = newEventData;
}

void Map::setStaticData(TileData* data)
{
	delete[] staticData;
	staticData = data;
}

void Map::setStaticTopData(TileData* data)
{
	delete[] staticTopData;
	staticTopData = data;
}

/*

IO to-do:

Save/load cutscenes

*/

bool Map::saveMapToFile(const char* path)
{
	std::ofstream ostream(path, std::ios::out | std::ios::binary | std::ios::trunc);
    
	if (ostream.is_open())
	{
		int size = height * width;
		int mapSize[2] = { width, height };
        
		ostream.write(reinterpret_cast<char*>(mapSize), 2 * sizeof(int));
		ostream.write(reinterpret_cast<char*>(staticData), size * sizeof(TileData));
		ostream.write(reinterpret_cast<char*>(staticTopData), size * sizeof(TileData));
        
		unsigned int portalCt = 0;
		for (unsigned int p = 0; p < size; p++)
		{
			PortalData* pd = &portalData[p];
			if (pd->isPortal) portalCt++;
		}
        
		//write portal count
		ostream.write(reinterpret_cast<char*>(&portalCt), sizeof(unsigned int));
        
		for (unsigned int p = 0; p < size; p++)
		{
			PortalData* pd = &portalData[p];
			if (pd->isPortal)
			{
				//write portal location
				ostream.write(reinterpret_cast<char*>(&p), sizeof(unsigned int));
                
				//write portal dest map len
				unsigned int mapFileLen = strlen(pd->destinationMap);
				ostream.write(reinterpret_cast<char*>(&mapFileLen), sizeof(unsigned int));
                
				//write portal dest map
				ostream.write(pd->destinationMap, mapFileLen);
                
				//write portal dest map location
				float pdLoc[2] = { pd->playerLoc.x, pd->playerLoc.y };
				char* pdd = reinterpret_cast<char*>(pdLoc);
				ostream.write(pdd, 2 * sizeof(float));
			}
		}
        
		//write is snowing
		ostream.write(reinterpret_cast<char*>(&isSnowing), sizeof(bool));
        
		//write dynamic count
		size_t dynSize = dynamicOffsets.size();
		ostream.write(reinterpret_cast<char*>(&dynSize), sizeof(size_t));
        
		//write dynamic data
		for (auto it = dynamicOffsets.begin(); it != dynamicOffsets.end(); it++)
		{
			float dynLoc[2] = { it->x, it->y };
			char* dynd = reinterpret_cast<char*>(dynLoc);
			ostream.write(dynd, 2 * sizeof(float));
            
			unsigned int i = it->y * width + it->x;
			unsigned char id = dynamicData[i].tileData.id;
			ostream.write(reinterpret_cast<char*>(&id), sizeof(unsigned char));
		}
        
		//write door size, write door vector
		size_t doorSize = doors.size();
		ostream.write(reinterpret_cast<char*>(&doorSize), sizeof(size_t));
        
		for (int x = 0; x < doors.size(); x++)
		{
			ostream.write(reinterpret_cast<char*>(&doors[x].position), sizeof(size_t));
			ostream.write(reinterpret_cast<char*>(&doors[x].openId), sizeof(unsigned char));
			ostream.write(reinterpret_cast<char*>(&doors[x].closedId), sizeof(unsigned char));
            ostream.write(reinterpret_cast<char*>(&doors[x].dir), 3 * sizeof(unsigned char));
            
			size_t reqSize = doors[x].requirements.size();
			ostream.write(reinterpret_cast<char*>(&reqSize), sizeof(size_t));
            
			for (int y = 0; y < reqSize; y++)
			{
				std::pair<size_t, unsigned char> req = doors[x].requirements[y];
				ostream.write(reinterpret_cast<char*>(&req.first), sizeof(size_t));
				ostream.write(reinterpret_cast<char*>(&req.second), sizeof(unsigned char));
			}
            
			//write position, animation, id
		}
        
		//write is cave
		ostream.write(reinterpret_cast<char*>(&isCave), sizeof(bool));
        
		//write events
		//std::cout << "size " << size << " bytes " << size * sizeof(TileEventData) << "\n";
		ostream.write(reinterpret_cast<char*>(eventData), size * sizeof(TileEventData));
        
		//write reset data
		ostream.write(reinterpret_cast<char*>(&resettable), sizeof(bool));
		ostream.write(reinterpret_cast<char*>(&resetPos), sizeof(glm::vec2));
        
		//write static layer 1 (not zero)
		ostream.write(reinterpret_cast<char*>(staticData1), size * sizeof(TileData));
        
		ostream.close();
		return true;
	}
    
	return false;
}

bool Map::loadMapFromFile(const char* path)
{
	int mapSize[2];
	int oldSize = size;
    
	std::cout << "Loading map " << path << "\n";
	std::ifstream istream(path, std::ios::in | std::ios::binary);
    
	if (istream.is_open())
	{
		char dataSize[2 * sizeof(int)];
        
		istream.read(dataSize, 2 * sizeof(int));
		memcpy(&mapSize[0], dataSize, sizeof(int));
		memcpy(&mapSize[1], dataSize + sizeof(int), sizeof(int));
        
		width = mapSize[0];
		height = mapSize[1];
        
		size = height * width;
        
		std::cout << "Map dimensions: " << width << "x" << height << "\n";
        
		char* data = new char[size * sizeof(TileData)];
		char* dataTop = new char[size * sizeof(TileData)];
        
		istream.read(data, size * sizeof(TileData));
		istream.read(dataTop, size * sizeof(TileData));
        
		delete[] staticData1;
		delete[] staticData1Colors;
        delete[] staticDataColors;
        staticData1 = new TileData[size];
        staticData1Colors = new glm::vec3[size];
		staticDataColors = new glm::vec3[size];
        memset(staticData1, 0, sizeof(TileData) * size);
        setStaticData(reinterpret_cast<TileData*>(data));
		setStaticTopData(reinterpret_cast<TileData*>(dataTop));
        
		//clear dynamic data and claimed data
		delete[] dynamicData;
		delete[] claimedData;
		dynamicOffsets.clear();
		dynamicUvs.clear();
        
		//set default dynamic data and set claimed tiles to false
		dynamicData = new DynamicData[size];
		claimedData = new bool[size];
		for (unsigned long x = 0; x < size; x++)
		{
			TileData d{};
			DynamicData dd{};
            
			dd.tileData = d;
            
			dynamicData[x].tileData = d;
			claimedData[x] = false;
		}
		
		mapFile = std::string(path);
        
		//delete portal data
		for (int x = 0; x < oldSize; x++)
		{
			if (portalData[x].isPortal)
			{
				delete[] portalData[x].destinationMap;
			}
		}
		delete[] portalData;
		
		//set default portal data
		portalData = new PortalData[size]; //TODO: copy in portal data from file
		for (int x = 0; x < size; x++)
		{
			PortalData pd{};
			portalData[x] = pd;
		}
        
		
		//read portal count
		char* portalDataTemp = new char[sizeof(unsigned int)];
        
		istream.read(portalDataTemp, sizeof(unsigned int));
		unsigned int portalCt = 0;
		memcpy(&portalCt, portalDataTemp, sizeof(unsigned int));
        
		delete[] portalDataTemp;
        
		//read each portal
		for (int p = 0; p < portalCt; p++)
		{
			//read portal index
			portalDataTemp = new char[sizeof(unsigned int)];
			istream.read(portalDataTemp, sizeof(unsigned int));
			unsigned int pi = 0;// reinterpret_cast<unsigned int*>(portalDataTemp)[0];
			memcpy(&pi, portalDataTemp, sizeof(unsigned int));
			delete[] portalDataTemp;
            
			//read portal dest map len
			portalDataTemp = new char[sizeof(unsigned int)];
			istream.read(portalDataTemp, sizeof(unsigned int));
			unsigned int destMapLen = 0;// static_cast<unsigned int>(portalDataTemp[0]);
			memcpy(&destMapLen, portalDataTemp, sizeof(unsigned int));
			delete[] portalDataTemp;
            
			//read portal destination map
			char* destMap = new char[destMapLen+1];
			istream.read(destMap, destMapLen);
			destMap[destMapLen] = '\0';
            
			//read portal dest map loc
			portalDataTemp = new char[2 * sizeof(float)];
			istream.read(portalDataTemp, 2 * sizeof(float));
			float* destLoc = reinterpret_cast<float*>(portalDataTemp);

			PortalData* pd = &portalData[pi];
			pd->destinationMap = destMap;
			pd->isPortal = true;
			pd->playerLoc = glm::vec2(destLoc[0], destLoc[1]);

			delete[] portalDataTemp;
		}
		
		//read is snowing
		char isSnowingData;
		istream.read(&isSnowingData, 1);
		isSnowing = static_cast<bool>(isSnowingData);
        
		//read dynamic tile count
		char* dynData = new char[sizeof(size_t)];
		istream.read(dynData, sizeof(size_t));
		size_t dynCt = 0;// static_cast<size_t>(dynData[0]);
		memcpy(&dynCt, dynData, sizeof(size_t));
		delete[] dynData;
        
		//read each dynamic tile
		for (size_t i = 0; i < dynCt; i++)
		{
			//read offset
			dynData = new char[2 * sizeof(float)];
			istream.read(dynData, 2 * sizeof(float));
			float* dynLocTemp = reinterpret_cast<float*>(dynData);
			float dynLoc[2] = { dynLocTemp[0], dynLocTemp[1] };
			delete[] dynData;
            
			//read id
			dynData = new char[sizeof(unsigned char)];
			istream.read(dynData, sizeof(unsigned char));
			unsigned char ddId = static_cast<unsigned char>(dynData[0]);
			delete[] dynData;
            
			setTileDynamic(dynLoc[0], dynLoc[1], ddId);
		}
        
		//read door stuff
		char doorSize[sizeof(size_t)];
		istream.read(doorSize, sizeof(size_t));
		size_t doorSizeT = 0;// static_cast<size_t>(doorSize[0]);
		memcpy(&doorSizeT, doorSize, sizeof(size_t));
        
		doors.clear();
		for (int x = 0; x < doorSizeT; x++)
		{
			DoorData door;
            
			char doorPos[sizeof(size_t)];
			char openId[sizeof(unsigned char)];
			char closedId;
			char dir[3];
            char reqSize[sizeof(size_t)];
            
			istream.read(doorPos, sizeof(size_t));
			istream.read(openId, sizeof(unsigned char));
			istream.read(&closedId, sizeof(unsigned char));
			istream.read(dir, sizeof(unsigned char) * 3);
            istream.read(reqSize, sizeof(size_t));
            
			size_t reqSizeT = 0;// static_cast<size_t>(reqSize[0]);
			memcpy(&reqSizeT, reqSize, sizeof(size_t));
			
			//door.position = static_cast<size_t>(doorPos[0]);
			memcpy(&door.position, doorPos, sizeof(size_t));
			door.openId = static_cast<unsigned char>(openId[0]);
			door.isOpen = false;
            
			door.closedId = closedId;
            memcpy(door.dir, dir, sizeof(unsigned char) * 3);
            
			for (int y = 0; y < reqSizeT; y++)
			{
				char reqPos[sizeof(size_t)];
				char reqId[sizeof(unsigned char)];
                
				istream.read(reqPos, sizeof(size_t));
				istream.read(reqId, sizeof(unsigned char));
                
				size_t reqPosT = 0;
				memcpy(&reqPosT, reqPos, sizeof(size_t));
				std::pair<size_t, unsigned char> req = { reqPosT , static_cast<unsigned char>(reqId[0]) };
				door.requirements.push_back(req);
			}
            
			doors.push_back(door);
		}
        
		//read is cave
		char isCaveData;
		istream.read(&isCaveData, 1);
		this->isCave = static_cast<bool>(isCaveData);
        
		//read event data
		delete[] eventData;
		char* tempEventData = new char[size * sizeof(TileEventData)];
		istream.read(tempEventData, size * sizeof(TileEventData));
		eventData = reinterpret_cast<TileEventData*>(tempEventData);
        
		//read reset data
		istream.read(reinterpret_cast<char*>(&resettable), sizeof(bool));
		istream.read(reinterpret_cast<char*>(&resetPos), sizeof(glm::vec2));
        
		//read static layer 1 (not zero)
		istream.read(reinterpret_cast<char*>(staticData1), size * sizeof(TileData));
        for(int i=0; i < size; i++)
        {
            staticData1Colors[i] = glm::vec3(1);
        }
        
        for(int i=0; i < size; i++)
        {
            staticDataColors[i] = glm::vec3(1);
        }
        
		istream.close();
        
#ifdef DEBUG_BUILD
		this->justLoaded = true;
#endif
		
		/* Reset undo buffer indices */
		stampNum = 0;
		stampLast = 0;
		stampBtm = 0;
        
		return true;
	}
	
	std::cout << "Map failed to load " << path << "\n";
	return false;
}

const char* Map::getMapFileName()
{
	return mapFile.c_str();
}