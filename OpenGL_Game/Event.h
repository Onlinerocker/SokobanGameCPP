#pragma once
#include <vector>

class MapScene;
//#include "MapScene.h"

class Event
{
    
    public:
    virtual void setupEvent(MapScene& scene) = 0;
	virtual void runEvent(MapScene& scene, float deltaTime) = 0;
    
};

