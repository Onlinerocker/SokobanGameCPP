#pragma once
#include "../Event.h"
#include "../QuadAnimated.h"
#include "../WorldEnums.h"

#define EV_TEST_PATH_LEN 20

class MapScene;

typedef WorldEnums::WRL_DIR Dir;

class EventTest
{
    
    private:
	EventTest();
	static bool setup;
	static bool setupDialogue;
    static size_t curPath;
	static Dir path[EV_TEST_PATH_LEN];
    
    public:
	static void setupEvent(MapScene& scene);
	static bool runEvent(MapScene& scene, float deltaTime);
    
};

