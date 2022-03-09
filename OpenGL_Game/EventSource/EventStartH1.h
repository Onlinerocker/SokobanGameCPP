#pragma once

#include "../Event.h"
#include "../QuadAnimated.h"
#include "../WorldEnums.h"

class MapScene;

typedef WorldEnums::WRL_DIR Dir;

class EventStartH1
{
    
    private:
	EventStartH1();
    
    public:
	static void setupEvent(MapScene& scene);
	static bool runEvent(MapScene& scene, float deltaTime);
    
};
