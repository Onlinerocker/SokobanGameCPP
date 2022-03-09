#pragma once

#include "../Event.h"
#include "../QuadAnimated.h"
#include "../WorldEnums.h"

#define EV_PATH_LEN 1

class MapScene;

typedef WorldEnums::WRL_DIR Dir;

class EventIntro
{
    
    private:
	EventIntro();
    static bool setup;
	static size_t curPath;
	static Dir path[EV_PATH_LEN];
    
    public:
	static void setupEvent(MapScene& scene);
	static bool runEvent(MapScene& scene, float deltaTime);
    
};
