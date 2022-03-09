#include "../MapScene.h"
#include "GL/glew.h"
#include "../DialogueBuilder.h"


/* Event test static state */
bool EventTest::setup = false;
bool EventTest::setupDialogue = false;
size_t EventTest::curPath = 0;
Dir EventTest::path[EV_TEST_PATH_LEN] = 
{ 
    Dir::DOWN, 
    Dir::DOWN, 
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT,
    Dir::RIGHT
};

/* Static functions */
void EventTest::setupEvent(MapScene& scene)
{
    curPath = 0;
    setup = false;
    
    //TODO this is a memory leak
	QuadAnimated* quadAnim = new QuadAnimated(scene.assMan->textures[ASSET_EMELIA_WALK].id, 1.0f, 4, glm::vec3(0.0f, -8.0f, 0.0f), glm::vec2(17.0f, 25.0f), glm::vec2(0.25f, 1.0f / 4.0f));
	Player p(glm::vec3(30.0f * 17.0f, 12.0f * 17.0f, 0), 17.0f, nullptr, *quadAnim);
	p.isNPC = true;
    
    NPC npcEmelia{quadAnim, p};
	scene.npcs.push_back(npcEmelia);
    
	Player& pl = scene.npcs[scene.npcs.size() - 1].second;
    
	pl.setupMoving(Dir::UP);
	//pl.startMoving(WorldEnums::WRL_DIR::RIGHT, scene->map);
    
	DialogueBox* diaBox = scene.diaBox;
    diaBox->dialogueNames.clear();
    diaBox->dialoguePairs.clear();
    DialogueBuilder::build(scene.assMan, diaBox, "Text/emelia_intro.txt");
    
	diaBox->startDialogue();
	diaBox->enabled = false;
    
	scene.player->isTalking = true;
    scene.player->setupMoving(Dir::DOWN);
}

bool EventTest::runEvent(MapScene& scene, float deltaTime)
{
    if (scene.fadeTime > 0.0f) return true;
    else if(!setupDialogue)
    {
        scene.diaBox->enabled = true;
        setupDialogue = true;
    }
    
    if(setupDialogue && scene.diaBox->enabled) return true;
    
	if (scene.npcs.size() <= 0) return false;
	Player& p = scene.npcs[scene.npcs.size() - 1].second;
	
	if (!EventTest::setup)
	{
		p.setupMoving(path[0]);
		p.startMoving(path[0], *scene.map, scene.part);
		EventTest::setup = true;
        scene.player->setupMoving(Dir::DOWN);
        curPath++;
	}
    
	if (!p.doMoving(*scene.map, deltaTime, scene.part) && curPath < EV_TEST_PATH_LEN)
	{
		glm::vec2 worldPos = p.worldPos;
		glm::vec2 offset(0);
		Dir curDir = EventTest::path[curPath];
        
		switch (curDir)
		{
            
            case Dir::RIGHT:
			offset = glm::vec2(1, 0);
			break;
            case Dir::LEFT:
			offset = glm::vec2(-1, 0);
			break;
            case Dir::UP:
			offset = glm::vec2(0, -1);
			break;
            case Dir::DOWN:
			offset = glm::vec2(0, 1);
			break;
            
		}
        
		if (scene.map->canMoveIntoWorldPos(worldPos, glm::vec2(worldPos) + offset))
		{
			curPath++;
            if(curPath < EV_TEST_PATH_LEN)
            {
                p.setupMoving(curDir);
                p.startMoving(curDir, *scene.map, scene.part);
            }
        }
        else
        {
            curPath = EV_TEST_PATH_LEN;
        }
    }
    else if (curPath >= EV_TEST_PATH_LEN)
    {
        //can't do erase, requries default ctor, which we can't really do, becuase
        //quad animated is a reference in player... FML, don't code like, it's dumb #pain
        //scene.npcs.erase(scene.npcs.begin() + (scene.npcs.size() - 1));
        //scene.npcs.pop_back();
        
        glm::vec2 pos = p.worldPos - glm::vec2(scene.map->worldPos.x, scene.map->worldPos.y);
        scene.map->setClaimTileStatic(pos.x, pos.y, false);
        scene.npcs.erase(scene.npcs.begin() + (scene.npcs.size() - 1));
        scene.player->isTalking = false;
        return false;
    }
    return true;
}