#include "Player.h"

#include <iostream>

Player::Player(glm::vec3 pos, float tileSize, InputHandler* inputHandler, QuadAnimated& quadAnim) :
quadAnim(quadAnim)
{
	isTalking = false;
	isMoving = false;
    isNPC = false;
    isAlive = true;
    isAttacking = false;
    tileDelta = 0.0f;
    timeOffset = 0.0f;
    attackTime = 0.0f;
    partDelay[0] = 0.0f;
    partDelay[1] = 0.0f;
    playerAnimTime = 0.0f;
    oldPos = 0.0f;
    curDir = Dir::DOWN;
    timeMovement = 0.0f;
    timeGlow = 0.0f;
    sprintSpeed = 1.5f;
    
	this->pos = pos;
    this->destPos = pos;
    this->worldPos = glm::floor(pos / tileSize);
    this->tileSize = tileSize;
	this->inputHandler = inputHandler;
    
    this->quadAnim.getAnimationController()->addState("w_down", 4, 1.0f, 0.0f, false);
    this->quadAnim.getAnimationController()->setState("w_down");
    this->quadAnim.getAnimationController()->addState("w_up", 4, 1.0f, 1.0f / 4.0f, false);
    this->quadAnim.getAnimationController()->addState("w_right", 4, 1.0f, 2.0f / 4.0f, false);
    this->quadAnim.getAnimationController()->addState("w_left", 4, 1.0f, 3.0f / 4.0f, false);
}

/*Player::Player() : quadAnim()
{
    isTalking = false;
	isMoving = false;
    isNPC = false;
    tileDelta = 0.0f;
    timeOffset = 0.0f;
    playerAnimTime = 0.0f;
    oldPos = 0.0f;
    curDir = Dir::DOWN;
    timeMovement = 0.0f;
    timeGlow = 0.0f;
    sprintSpeed = 1.5f;
    
    this->quadAnim.getAnimationController()->addState("w_down", 4, 1.0f, 0.0f, false);
    this->quadAnim.getAnimationController()->setState("w_down");
    this->quadAnim.getAnimationController()->addState("w_up", 4, 1.0f, 1.0f / 4.0f, false);
    this->quadAnim.getAnimationController()->addState("w_right", 4, 1.0f, 2.0f / 4.0f, false);
    this->quadAnim.getAnimationController()->addState("w_left", 4, 1.0f, 3.0f / 4.0f, false);
}*/

void Player::updateDynamicTile(Map& map, bool done, glm::vec2 direction, Particle& part)
{
    glm::vec2 partOff(0,-5.0f);
    
    //partOff.y += 2.0f;
    if(direction.y < 0) partOff.y -= 8.0f;
    if(direction.x < 0) partOff.x -= 0.0f;
    
    for (int x = 0; x < 2; x++)
    {
        if (ddPushing[x].tileData.id > 0)
        {
            glm::vec2& p = map.dynamicOffsets[ddPushing[x].offsetIndex];
            if (!done) p = dynamicOldPos[x] + (glm::vec2(tileDelta / tileSize) * direction);
            else if (done)
            {
                p = dynamicOldPos[x] + direction;
                
                unsigned int iNew = (dynamicOldPos[x].y + direction.y) * map.width + (dynamicOldPos[x].x + direction.x);
                unsigned int i = dynamicOldPos[x].y * map.width + dynamicOldPos[x].x;
                
                map.dynamicData[iNew] = map.dynamicData[i];
                map.dynamicData[i] = DynamicData{};
                
                ddPushing[x] = DynamicData{};
                partDelay[0] = 0.0f;
                partDelay[1] = 0.0f;
            }
            
            if(1.0 != partDelay[x])
            {
                //part.addParticles(p*17.0f + partOff, 4, 65, map.isSnowing && !map.isCave ? glm::vec3(20) : glm::vec3(1));
                partDelay[x] = 1.0;//glm::floor(tileDelta / 17.0f);
            }
        }
    }
    
}

bool Player::updatePosition(float deltaTime, Map& map, Particle& part)
{
    if (inputHandler && inputHandler->isShiftDown) deltaTime *= sprintSpeed;
    
    bool done = false;
    
    tileDelta += deltaTime * 60.0f;
    
    if (tileDelta >= tileSize)
    {
        tileDelta = tileSize;
        done = true;
    }
    
    playerAnimTime = tileDelta / tileSize / 2 + timeOffset;
    
    if (done) timeOffset = (timeOffset == 0.5f) ? 0.0f : 0.5f;
    
    switch (curDir)
    {
        case Dir::RIGHT:
        pos.x = oldPos + tileDelta;
        worldPos = glm::floor(pos / tileSize);
        
        updateDynamicTile(map, done, glm::vec2(1, 0), part);
        
        break;
        case Dir::LEFT:
        pos.x = oldPos - tileDelta;
        worldPos = glm::ceil(pos / tileSize);
        
        updateDynamicTile(map, done, glm::vec2(-1, 0), part);
        
        break;
        case Dir::UP:
        {
            pos.y = oldPos - tileDelta;
            worldPos = glm::ceil(pos / tileSize);
            
            updateDynamicTile(map, done, glm::vec2(0, -1), part);
            
            break;
        }
        case Dir::DOWN:
        pos.y = oldPos + tileDelta;
        worldPos = glm::floor(pos / tileSize);
        
        updateDynamicTile(map, done, glm::vec2(0, 1), part);
        
        break;
    }
    
    return done;
}

void Player::setupMoving(Dir dir)
{
    curDir = dir;
    
    switch (dir)
    {
        
        case Dir::RIGHT:
        quadAnim.getAnimationController()->setState("w_right");
        break;
        case Dir::LEFT:
        quadAnim.getAnimationController()->setState("w_left");
        break;
        case Dir::UP:
        quadAnim.getAnimationController()->setState("w_up");
        break;
        case Dir::DOWN:
        quadAnim.getAnimationController()->setState("w_down");
        break;
        default:
        break;
        
    }
}

void Player::startMoving(WorldEnums::WRL_DIR dir, Map& map, Particle& part)
{
    isMoving = true;
    tileDelta = 0.0f;
    glm::vec2 offset(0,0);
    glm::vec2 partOff(0,0);
    
    switch (dir)
    {
        
        case Dir::RIGHT:
        offset.x = 1;
        oldPos = pos.x;
        break;
        case Dir::LEFT:
        offset.x = -1;
        oldPos = pos.x;
        partOff.x -= 8.0f;
        break;
        case Dir::UP:
        offset.y = -1;
        oldPos = pos.y;
        partOff.y -= 8.0f;
        break;
        case Dir::DOWN:
        offset.y = 1;
        oldPos = pos.y;
        break;
        default:
        break;
        
    }
    
    if (!isNPC)
    {
        StateStamp* stateStamps = map.stateStamps;
        StateStamp state{};
        state.playerFrom = worldPos;
        state.directionFrom = curDir;
        state.dynamicOffsets = map.dynamicOffsets;
        stateStamps[map.stampNum] = state;
        memcpy(stateStamps[map.stampNum].dynamicData, map.dynamicData, (map.width * map.height) * sizeof(DynamicData));
        memcpy(stateStamps[map.stampNum].claimedData, map.claimedData, (map.width * map.height) * sizeof(bool));
        memcpy(stateStamps[map.stampNum].staticData1, map.staticData1, (map.width * map.height) * sizeof(TileData));
        
        if (map.stampNum <= map.stampLast)
        {
            map.stampBtm = map.stampNum;
        }
        
        map.stampLast = map.stampNum;
        map.stampNum++;
    }
    
    map.setClaimTileStatic(worldPos.x - map.worldPos.x, worldPos.y - map.worldPos.y, false);
    map.setClaimTileStatic(worldPos.x - map.worldPos.x + offset.x, worldPos.y - map.worldPos.y + offset.y, true);
    
    ddPushing[1] = map.getTileDynamicWorldPos(worldPos.x + offset.x, worldPos.y + offset.y);
    if (ddPushing[1].tileData.id > 0) 
    {
        dynamicOldPos[1] = (map.dynamicOffsets)[ddPushing[1].offsetIndex];
        //part.addParticles(dynamicOldPos[1]*17.0f + partOff, 16, 84, map.isSnowing && !map.isCave ? glm::vec3(20) : glm::vec3(1));
        
        ddPushing[0] = map.getTileDynamicWorldPos(worldPos.x + (offset.x*2), worldPos.y + (offset.y*2));
        if (ddPushing[0].tileData.id > 0 && ddPushing[1].tileData.id >= 59)
        {
            dynamicOldPos[0] = (map.dynamicOffsets)[ddPushing[0].offsetIndex];
            //part.addParticles(dynamicOldPos[0]*17.0f + partOff, 16, 84, map.isSnowing && !map.isCave ? glm::vec3(20) : glm::vec3(1));
        }
        else
        {
            ddPushing[0] = {};
        }
    }
}

bool Player::doMoving(Map& map, float deltaTime, Particle& part)
{
    return !updatePosition(deltaTime, map, part);
}

void Player::updatePlayer(float deltaTime, Map& map, Particle& part)
{
    if (!inputHandler || deltaTime == 0.0f) return;
    
    for (auto& it : map.dynamicGlowing)
    {
        it = 0.0f;
    }
    
    bool isDyn = false;
    auto isDynAndGlow = [](Map& map, const glm::vec2& worldPos, const glm::vec2& dir, float& timeGlow, float deltaTime)
    {
        DynamicData d = map.getTileDynamicWorldPos(worldPos.x + dir.x, worldPos.y + dir.y);
        if (d.tileData.id > 0)
        {
            map.dynamicGlowing[d.offsetIndex] = 1.0f;
            
            if (d.tileData.id >= 59)
            {
                DynamicData d1 = map.getTileDynamicWorldPos(worldPos.x + (dir.x*2), worldPos.y + (dir.y*2));
                if (d1.tileData.id > 0)
                {
                    map.dynamicGlowing[d1.offsetIndex] = 1.0f;
                }
            }
            
            timeGlow += deltaTime;
            return true;
        }
        return false;
    };
    
    if (curDir == Dir::UP)
    {
        isDyn = isDynAndGlow(map, worldPos, glm::vec2(0, -1), timeGlow, deltaTime);
    }
    else if (curDir == Dir::DOWN)
    {
        isDyn = isDynAndGlow(map, worldPos, glm::vec2(0, 1), timeGlow, deltaTime);
    }
    else if (curDir == Dir::LEFT)
    {
        isDyn = isDynAndGlow(map, worldPos, glm::vec2(-1, 0), timeGlow, deltaTime);
    }
    else if (curDir == Dir::RIGHT)
    {
        isDyn = isDynAndGlow(map, worldPos, glm::vec2(1, 0), timeGlow, deltaTime);
    }
    
    if (!isDyn)
    {
        timeGlow = 0.0f;
    }
    
    if(isTalking || !isAlive) 
    {
        return;
    }
    
    if(isAttacking)
    {
        attackTime += deltaTime;
        std::cout << "attack time " << attackTime << "\n";
        if(attackTime >= 0.333f) //this will depend on the animation time length
        {
            isAttacking = false;
        }
    }
    
    if (isMoving)
    {
        bool isMoveTemp = isMoving;
        isMoving = doMoving(map, deltaTime, part);
        if (isMoving || isTalking) return;
        else if (isMoveTemp != isMoving)
        {
            return;
        }
    }
    
    /* UP MOVEMENT */
    bool up = inputHandler->isWDown && inputHandler->lastDirs[0] == Dir::UP;
    if (up)
    {
        setupMoving(Dir::UP);
        timeMovement += deltaTime;
    }
    if (up && map.canMoveIntoWorldPos(worldPos, glm::vec2(worldPos.x, worldPos.y - 1)) && timeMovement >= MOVEMENT_DELAY)
    {
        destPos = pos + glm::vec3(0.0f, -17.0f, 0.0f);
        startMoving(curDir, map, part);
    }
    
    /* DOWN MOVEMENT */
    bool down = inputHandler->isSDown && inputHandler->lastDirs[0] == Dir::DOWN;
    if (down)
    {
        setupMoving(Dir::DOWN);
        timeMovement += deltaTime;
    }
    if (down && map.canMoveIntoWorldPos(worldPos, glm::vec2(worldPos.x, worldPos.y + 1)) && timeMovement >= MOVEMENT_DELAY)
    {
        destPos = pos + glm::vec3(0.0f, 17.0f, 0.0f);
        startMoving(curDir, map, part);
    }
    
    /* RIGHT MOVEMENT */
    bool right = inputHandler->isDDown && inputHandler->lastDirs[0] == Dir::RIGHT;
    if (right)
    {
        setupMoving(Dir::RIGHT);
        timeMovement += deltaTime;
    }
    if (right && map.canMoveIntoWorldPos(worldPos, glm::vec2(worldPos.x + 1, worldPos.y)) && timeMovement >= MOVEMENT_DELAY)
    {
        destPos = pos + glm::vec3(17.0f, 0.0f, 0.0f);
        startMoving(curDir, map, part);
    }
    
    /* LEFT MOVEMENT */
    bool left = inputHandler->isADown && inputHandler->lastDirs[0] == Dir::LEFT;
    if (left)
    {
        setupMoving(Dir::LEFT);
        timeMovement += deltaTime;
    }
    if (left && map.canMoveIntoWorldPos(worldPos, glm::vec2(worldPos.x - 1, worldPos.y)) && timeMovement >= MOVEMENT_DELAY)
    {
        destPos = pos + glm::vec3(-17.0f, 0.0f, 0.0f);
        startMoving(curDir, map, part);
    }
    
    if (!up && !down && !left && !right)
    {
        timeMovement = 0.0f;
        
        //if j is down and not attacking... attackign = true
        if(inputHandler->isEnterDown && !isAttacking)
        {
            isAttacking = true;
            attackTime = -deltaTime;
        }
    }
    
}

void Player::setPos(glm::vec3 pos)
{
    isMoving = false;
    tileDelta = 0.0f;
    this->pos = pos;
    this->worldPos = glm::floor(pos / tileSize);
}

/*Player& Player::operator=(Player&&)
{
    return *this;
}*/


/*float Player::getAnimTime()
{
    return playerAnimTime;
}

glm::vec3 Player::getPos()
{
    return pos;
}

glm::vec2 Player::getWorldPos()
{
    return worldPos;
}

Dir Player::getCurDir()
{
    return curDir;
}*/