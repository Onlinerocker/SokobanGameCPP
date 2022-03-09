#include "EventStartH1.h"
#include "../DialogueBox.h"
#include "../DialogueBuilder.h"
#include "../MapScene.h"

#include <iostream>

void EventStartH1::setupEvent(MapScene& scene)
{
    scene.changeToCutscene = true;
}

bool EventStartH1::runEvent(MapScene& scene, float deltaTime)
{
	return true;
}