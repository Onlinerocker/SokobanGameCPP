#include "EventIntro.h"
#include "../DialogueBox.h"
#include "../DialogueBuilder.h"
#include "../MapScene.h"

#include <iostream>

bool EventIntro::setup = false;
size_t EventIntro::curPath = 0;
Dir EventIntro::path[EV_PATH_LEN] =
{
    Dir::RIGHT
};

void EventIntro::setupEvent(MapScene& scene)
{
    setup = false;
    curPath = 0;
    
    DialogueBox* diaBox = scene.diaBox;
    diaBox->dialogueNames.clear();
    diaBox->dialoguePairs.clear();
    DialogueBuilder::build(scene.assMan, diaBox, "Text/mc_intro.txt");
    
    //diaBox->addDialogue("\u005a\u005a\u005a", "test", -1, false);
	diaBox->startDialogue();
	diaBox->enabled = true;
    
	scene.player->isTalking = true;
    scene.player->setupMoving(Dir::DOWN);
}

bool EventIntro::runEvent(MapScene& scene, float deltaTime)
{
    if (!scene.player) return false;
	Player& p = *scene.player;
    
    if (scene.diaBox->enabled) return true;
    else if (!setup)
	{
		p.setupMoving(path[0]);
		p.startMoving(path[0], *scene.map, scene.part);
		setup = true;
        curPath++;
	}
    
    if (!p.doMoving(*scene.map, deltaTime, scene.part) && curPath >= EV_PATH_LEN)
	{
        scene.player->isTalking = false;
		return false;
	}
    
	return true;
}