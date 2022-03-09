#pragma once

#include "glm/glm.hpp"

#include "InputHandler.h"
#include "WorldEnums.h"
#include "QuadAnimated.h"
#include "Map.h"
#include "Particle.h"

#define MOVEMENT_DELAY 0.125f

class Player
{
    typedef WorldEnums::WRL_DIR Dir;
    
    private:
	float oldPos;
	float timeOffset;
	float timeMovement;
    
	glm::vec2 dynamicOldPos[2];
    
	InputHandler* inputHandler;
    
	bool updatePosition(float deltaTime, Map& map, Particle& part);
	void updateDynamicTile(Map& map, bool done, glm::vec2 direction, Particle& part);
    
    public:
	bool isTalking;
	bool isMoving;
	bool isNPC;
    bool isAlive;
    bool isAttacking;
	float playerAnimTime, timeGlow, sprintSpeed, attackTime;
	float tileSize;
	float tileDelta;
    float partDelay[2];
    
    Dir curDir;
	glm::vec3 pos;
    glm::vec3 destPos;
	glm::vec2 worldPos;
	DynamicData ddPushing[2];
	QuadAnimated& quadAnim;
    
	Player(glm::vec3 pos, float tileSize, InputHandler* inputHandler, QuadAnimated& quadAnim);
    //Player();
    
	void setupMoving(Dir dir);
	void startMoving(Dir dir, Map& map, Particle& part);
	bool doMoving(Map& map, float deltaTime, Particle& part);
	void updatePlayer(float deltaTime, Map& map, Particle& part);
	void setPos(glm::vec3 pos);
    
    //Player& operator=(Player&&);
};

