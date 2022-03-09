#include "MapScene.h"
#include "TextureUtil.h"

#include "glm/glm.hpp"
#include "stb_image_write.h"

#include <algorithm>
#include <iostream>
#include <random>

MapScene::MapScene(Map* map, GLuint mapTexture, MusicManager* musicManager, AssetManager* assMan, SDL_DisplayMode displayMode, int width, int height, float pixelWidth, float pixelHeight)
: 
mapRenderer(map, mapTexture),
quadAnim(assMan->textures[ASSET_PLAYER_WALK].id, 1.0f, 4, glm::vec3(0.0f, -8.0f, 0.0f), glm::vec2(17.0f, 25.0f), glm::vec2(0.25f, 1.0f / 4.0f)),
cam(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, glm::vec3(0.0, 1.0f, 0.0f)),
eventMachine(),
part(0, *map->tileManager),
centerText("", assMan->fonts[FONT_ARIAL_BIG], 650, 700, width, height, width, height),
centerTextOut("", assMan->fonts[FONT_ARIAL_BIG_OUT], 650, 700, width, height, width, height),
deathSpriteQuad("Images/gray_player.png", glm::vec3(0), glm::vec2(34,25), glm::vec2(1))
{
	this->type = 0;
	this->map = map;
	this->musicManager = musicManager;
	this->assMan = assMan;
	this->displayMode = displayMode;
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	ended = false;
	isLoading = false;
	changeToCutscene = false;
    shaking = false;
    curEvent = nullptr;
    
    lastEvent = 0;
	doorFrame = 0;
    
	time = 0.0f;
	lastCamUpdate = 0.0f;
    noCamUpdateFrames = 1.0f;
    fadeTime = 1.0f;
    camTime = 0.0f;
	weatherTimeX = 0.0f;
	weatherTimeY = 0.0f;
    doorParticleDelay = 0.0f;
    doorParticleDelayInterval = 0.2f;
    shakeTime = 0.0f;
    soulTime = 0.0f;
    lastSnowTime = glm::vec2(0);
    
    for(int i=0; i < 128; i++)
    {
        completedEvents[i] = false;
    }
    
    const char* deathStr = "YOU DIED";
    memset(deathText, 0, 2*32);
    for(int i=0; i < 8; i++)
    {
        deathText[i] = (Uint16)deathStr[i];
        deathImg = centerText.createUtf8Image("YOU DIED", &wDeath, &hDeath);
        
        glm::vec2 uvs[4] = { glm::vec2(0), glm::vec2(0, 1), glm::vec2(1,1), glm::vec2(1,0) };
        deathQuad.setQuad(glm::vec2((width - wDeath) / 2.0f, (height - hDeath) / 2.0f), glm::vec2(wDeath, hDeath), uvs, width, height);
        
        deathImgOut = centerTextOut.createUtf8Image("YOU DIED", &wDeath, &hDeath);
    }
    
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	proj = glm::ortho<float>(0.0f, pixelWidth, pixelHeight, 0.0f, 0.1f, 100.0f);
    
	playerWorldPos = glm::vec2(0);
    //test
	/* RESOURCES */
    
	/* Textures */
	textures["mapTexture"] = mapTexture;
	textures["textureRandom"] = assMan->textures[ASSET_RANDOM].id;// TextureUtil::createTexture("Images/random.jpg", false, GL_NEAREST);
	textures["textureNoise"] = assMan->textures[ASSET_NOISE].id;// TextureUtil::createTexture("Images/noise.jpg", false, GL_NEAREST);
	textures["textureAriz"] = assMan->textures[ASSET_ARIZ].id;// TextureUtil::createTexture("Images/mech.png", false, GL_LINEAR);
	textures["textureEmelia"] = assMan->textures[ASSET_EMELIA].id;// TextureUtil::createTexture("Images/emelia.png", false, GL_LINEAR);
    
	/* RESOURCES END */
    
	inputHandler = InputHandler::GetInputHandler();
	player = new Player(glm::vec3(0, 0, 0), 17.0f, inputHandler, quadAnim);
	diaBox = new DialogueBox(assMan->fonts[FONT_MSJH], assMan->fonts[FONT_ARIAL_I], 1920, 1080);
    
    
	camX = -((pixelWidth / 2) - 7.0f);
	camY = -((pixelHeight / 2));
	camZ = 1.0f;
    
	mapRenderer.initMeshParticle(5.0f);
	savedSnow = false;
	fastSnow = true;
    
	//TODO: Move to blob
	snowImg = TextureUtil::createTexture("Images/snow.png", true, GL_NEAREST);
}

MapScene::~MapScene()
{
	if (!ended) end();
}

void MapScene::cleanUpResources()
{
    
}

void MapScene::loadMapFromPortalData(const PortalData& d)
{
	newMapData = d;
	isLoading = true;
	fadeTime = 0.0f;
    
	std::cout << "Portal loading map: " << d.destinationMap << "!!\n";
}

bool MapScene::checkForPortal()
{
	glm::vec2 playerPos = player->worldPos;
	bool isPortal = false;
    
	if (playerPos != playerWorldPos)
	{
		PortalData d = map->getPortalAtWorldPos(playerPos.x, playerPos.y, &isPortal);
		if (isPortal)
		{
			loadMapFromPortalData(d);
		}
	}
    
	return isPortal;
}

void MapScene::killPlayer()
{
    player->isAlive = false;
    soulTime = 0.0f;
}

void MapScene::start() 
{
	if (!ended)
	{
		glDeleteFramebuffers(1, &frameBuffer);
		glDeleteTextures(1, &textureColorBuffer);
		glDeleteRenderbuffers(1, &rbo);
	}
    
	ended = false;
	/* SETUP FrameBuffer */
    
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL); //maybe this should be constant?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    
	/*glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080); // use a single renderbuffer object for both a depth AND stencil buffer. //consistent render target, fixing vertical black lines
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "[ERR]: FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
    
	/* END FrameBuffer setup */
}

glm::vec2 MapScene::calculateRelVelModifier(glm::vec2 velObj, glm::vec2 velParticle)
{
	glm::vec2 mod = velParticle - velObj;
	return mod;
}

void MapScene::run(float deltaTime)
{
	if (ended) return;
    
	//deltaTime *= 20.0f;
    
    printf("%f\n", newMapData.playerLoc.x);
    printf("BINGUS %f\n", map->portalData[45].playerLoc.x);

	if (isLoading || changeToCutscene)
	{
		if (fadeTime < 1.0f) fadeTime += 0.75*deltaTime;
		else if (fadeTime >= 1.0f && isLoading) 
		{
			if (map->loadMapFromFile(newMapData.destinationMap))
			{
				npcs.clear();
				refreshMap();
				player->setPos(glm::vec3(newMapData.playerLoc.x * 17.0f, newMapData.playerLoc.y * 17.0f, 0));
                player->isAlive = true;
                
                float destX = map->resettable ? (map->width/2 - 2) * 17.0f : player->pos.x;
                float destY = map->resettable ? (map->height/2 - 1) * 17.0f : player->pos.y;
                cam.setPosition(glm::vec3(camX + destX, 
                                          camY + destY, 
                                          camZ));
                
                playerWorldPos = player->worldPos;
				isLoading = false;
				fadeTime = 1.0f;
				curEvent = nullptr;
                
				npcs.clear();
			}
            
			if (isResetting)
			{
				delete[] resetName;
				isResetting = false;
			}
		} 
	}
	else if (fadeTime > 0.0f)
	{
		fadeTime -= 0.75*deltaTime;
		if (fadeTime < 0.0f)
		{
            isLoading = false;
			fadeTime = 0.0f;
		}
	}
    
	time += deltaTime;
    doorParticleDelay += deltaTime;
    
	//open and close doors accordingly
	size_t playerPos = (player->worldPos.y - map->worldPos.y) * map->width + (player->worldPos.x - map->worldPos.x);
	bool isSatisfied = true;
	bool toRefresh = false;
    
	for (auto it = map->doors.begin(); it != map->doors.end(); it++)
	{
		isSatisfied = true;
		size_t door_x = it->position % map->width;
		size_t door_y = it->position / map->width;
        unsigned char dir = it->dir[0];
        
		for (auto req = it->requirements.begin(); req != it->requirements.end(); req++)
		{
			size_t i = req->first;
			unsigned char id = req->second;
			if (map->dynamicData[i].tileData.id != id)
			{
				isSatisfied = false;
				break;
			}
		}
        
        if(isSatisfied != it->isOpen && isSatisfied)
        {
            shaking = true;
        }
        it->isOpen = isSatisfied;
        
        size_t start = dir == 0 ? door_y : door_x;
        size_t end = dir == 0 ? map->height : map->width;
        bool broke = false;
        
        size_t i = start;
        size_t tempX = dir == 0 ? door_x : i;
        size_t tempY = dir == 0 ? i : door_y;
        size_t p = tempY * map->width + tempX;
        
        unsigned char id = map->staticData1[p].id;
        unsigned char idStat = map->staticData[p].id;
        
        /*if(isSatisfied)
        {
            if(id != it->openId)
            {
                map->staticData1[p].id = it->openId;
                toRefresh = true;
            }
        }
        else
        {
            if(id != it->closedId)
            {
                map->staticData1[p].id = it->closedId;
                toRefresh = true;
            }
        }*/
        
        for (i = start; i < end; i++)
        {
            tempX = dir == 0 ? door_x : i;
            tempY = dir == 0 ? i : door_y;
            p = tempY * map->width + tempX;
            id = map->staticData1[p].id;
            
            if(isSatisfied)
            {
                if(it->closedId == 66)
                {
                    if(id == 66 || id == 255)
                    {
                        map->staticData1[p].id = it->openId;
                        toRefresh = true;
                    }else if(id == 254 || id == 253)
                    {
                        map->staticData1[p].id = 67;
                        toRefresh = true;
                    }
                }
                
                if(it->closedId == 67)
                {
                    if(id == 67 /* or == horizontal half id here*/ )
                    {
                        map->staticData1[p].id = it->openId;
                        toRefresh = true;
                    }else if(id == 254 || id == 253)
                    {
                        map->staticData1[p].id = 66;
                        toRefresh = true;
                    }
                }
            }
            else
            {
                if(it->closedId == 66)
                {
                    if(!broke)
                    {
                        if((id == 0 || id == 255) && (map->canWalkAtWorldPos(tempX + map->worldPos.x, tempY + map->worldPos.y) && id != 66) && 
                           playerPos != p && 
                           map->getTileDynamic(tempX, tempY).tileData.id == 0)
                        {
                            std::cout << "set at x " << tempX << " y " << tempY << "\n";
                            map->staticData1[p].id = 66;
                            toRefresh = true;
                        }else if(playerPos == p || map->getTileDynamic(tempX, tempY).tileData.id != 0)
                        {
                            if(id != 255)
                            {
                                map->staticData1[p].id = 255;
                                toRefresh = true;
                            }
                            
                            //draw particles
                            if(doorParticleDelay >= doorParticleDelayInterval)
                            {
                                glm::vec2 p(tempX + map->worldPos.x, tempY + map->worldPos.y);
                                glm::vec2 partOff(12.0f, 5.0f);
                                
                                part.addParticles(p*17.0f + partOff, 4, 65, map->isSnowing && !map->isCave ? glm::vec3(20) : glm::vec3(12.0f,1.0f,1.0f));
                                
                                doorParticleDelay = 0.0f;
                                
                                std::random_device rd;
                                std::mt19937 mt(rd());
                                std::uniform_real_distribution<float> dist(0.1f, 0.5f);
                                
                                doorParticleDelayInterval = dist(mt);
                            }
                            
                            broke = true;
                            //std::cout << "broke " << broke << "\n";
                        }else if(!map->canWalkAtWorldPos(tempX + map->worldPos.x, tempY + map->worldPos.y) && id != 66)
                        {
                            //std::cout << "can't walk at x " << tempX << " y " << tempY << "\n";
                            if(id != 253)
                            {
                                if(id == 67 && id != 254)
                                {
                                    map->staticData1[p].id = 254;
                                    toRefresh = true;
                                }
                                broke = true;
                            }
                            
                            if(i == start)
                            {
                                map->staticData1[p].id = 66;
                                toRefresh = true;
                            }
                        }
                    }
                    else if(broke && id == 66)
                    {
                        //std::cout << "broke at x " << tempX << " y " << tempY << "\n";
                        map->staticData1[p].id = it->openId;
                        toRefresh = true;
                    }
                }
                
                if(it->closedId == 67)
                {
                    if(!broke)
                    {
                        if((id == 0 || id == 252) && (map->canWalkAtWorldPos(tempX + map->worldPos.x, tempY + map->worldPos.y) && id != 67) && 
                           playerPos != p && 
                           map->getTileDynamic(tempX, tempY).tileData.id == 0)
                        {
                            std::cout << "set at x " << tempX << " y " << tempY << "\n";
                            map->staticData1[p].id = 67;
                            toRefresh = true;
                        }else if(playerPos == p || map->getTileDynamic(tempX, tempY).tileData.id != 0)
                        {
                            if(id != 253 && id != 252)
                            {
                                map->staticData1[p].id = 252;
                                toRefresh = true;
                            }
                            
                            //draw particles
                            if(doorParticleDelay >= doorParticleDelayInterval)
                            {
                                glm::vec2 p(tempX + map->worldPos.x, tempY + map->worldPos.y);
                                glm::vec2 partOff(12.0f, 5.0f);
                                
                                part.addParticles(p*17.0f + partOff, 4, 65, map->isSnowing && !map->isCave ? glm::vec3(20) : glm::vec3(12.0f,1.0f,1.0f));
                                
                                doorParticleDelay = 0.0f;
                                
                                std::random_device rd;
                                std::mt19937 mt(rd());
                                std::uniform_real_distribution<float> dist(0.1f, 0.5f);
                                
                                doorParticleDelayInterval = dist(mt);
                            }
                            
                            broke = true;
                            //std::cout << "broke " << broke << "\n";
                        }else if(!map->canWalkAtWorldPos(tempX + map->worldPos.x, tempY + map->worldPos.y) && id != 67)
                        {
                            //std::cout << "can't walk at x " << tempX << " y " << tempY << "\n";
                            /*if(id == 66 && id != 254)
                            {
                                map->staticData1[p].id = 254;
                                toRefresh = true;
                            }*/
                            //need id 253 for red over blue, should stop
                            if(id != 254)
                            {
                                if(id == 66 && id != 253)
                                {
                                    map->staticData1[p].id = 253;
                                    toRefresh = true;
                                }
                                broke = true;
                            }
                            
                            if(i == start)
                            {
                                map->staticData1[p].id = 67;
                                toRefresh = true;
                            }
                        }
                    }
                    else if(broke && id == 67)
                    {
                        std::cout << "h broke at x " << tempX << " y " << tempY << "\n";
                        map->staticData1[p].id = it->openId;
                        toRefresh = true;
                    }
                }
                
            }
        }
        
		/*if (!it->isOpen && (isSatisfied
                            || map->tileManager->isTileSolid(map->dynamicData[it->position].tileData.id) 
                            || playerPos == it->position))
		{
			it->isOpen = true;
			if (map->staticData1[it->position].id != it->openId)
			{
				map->staticData1[it->position].id = it->openId;
				toRefresh = true;
			}
			
            size_t start = dir == 0 ? door_y : door_x;
            size_t end = dir == 0 ? map->height : map->width;
            
			for (size_t i = start + 1; i < end; i++)
			{
                size_t tempX = dir == 0 ? door_x : i;
                size_t tempY = dir == 0 ? i : door_y;
				size_t p = tempY * map->width + tempX;
                
				if (it->closedId == map->staticData1[p].id || 255 == map->staticData1[p].id)
				{
					map->staticData1[p].id = it->openId;
					toRefresh = true;
				}
				else {
					//break;
				}
			}
		}
		else if (!isSatisfied)
		{
			bool broken = false;
			it->isOpen = false;
			if (map->staticData1[it->position].id != it->closedId && playerPos != (door_y * map->width + door_x) && map->getTileDynamic(door_x, door_y).tileData.id == 0)
			{
				map->staticData1[it->position].id = it->closedId;
				toRefresh = true;
			}
			else if(playerPos == (door_y * map->width + door_x) || map->getTileDynamic(door_x, door_y).tileData.id != 0){
				broken = true;
			}
            
            //If you switch the direction of a door, while unsatisfied, it won't clear the old openID tiles
            //Don't switch directions, while the door is unsatisfied!!!!!!
            
            size_t start = dir == 0 ? door_y : door_x;
            size_t end = dir == 0 ? map->height : map->width;
            
			for (size_t i = start + 1; i < end; i++)
			{
                size_t tempX = dir == 0 ? door_x : i;
                size_t tempY = dir == 0 ? i : door_y;
				size_t p = tempY * map->width + tempX;
                
                unsigned char id = map->staticData1[p].id;
                
				if ((map->canWalkAtWorldPos(tempX + map->worldPos.x, tempY + map->worldPos.y) || it->closedId == map->staticData1[p].id)
					&& map->getTileDynamic(tempX, tempY).tileData.id == 0
					&& playerPos != (p) && !broken)
				{
					if(id != 254 && it->closedId == 67)
                    {
                        map->staticData1[p].id = it->closedId;
                    }else if(it->closedId == 66){
                        std::cout << "x " << tempX << " y " << tempY << "\n";
                        std::cout << "id was " << (int)id << "\n";
                        map->staticData1[p].id = it->closedId;
                    }
					toRefresh = true;
				}
				else {
                    
                    //need to do this for blue lasers too!!
					if(map->staticData[p].id != 136 && map->staticData[p].id != 137 && 
                       ((map->staticData1[p].id != 66 && it->closedId == 67) || 
                        (map->staticData1[p].id != 67 && it->closedId == 66)))
                    {
                        if(!broken && it->closedId == 66 && (map->getTileDynamic(tempX, tempY).tileData.id != 0 || playerPos == p))
                        {
                            map->staticData1[p].id = 255; //magic id for half laser
                        }
                        
                    }
                    else if(!broken && (it->closedId == 66 && map->staticData1[p].id == 67))
                    {
                        std::cout << "x " << tempX << " y " << tempY << "\n";
                        map->staticData1[p].id = 254; //magic id for perpendicular lasers
                    } 
					broken = true;
				}		
			}
            
			
		}*/
	}
    
    
	if (toRefresh)
	{
		std::cout << "refreshing\n";
        refreshMap();
	}
    
    //std::cout << "end id is " << (int)map->staticData1[7 * map->width + 10].id << "\n";
    
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_FRAMEBUFFER_SRGB);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //consistent render target, fixing vertical black lines
	glClearColor(0.02f, 0.02f, 0.025f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	if (!isLoading) player->updatePlayer(deltaTime, *map, part);
	
    auto boolClamp = [](float val, float min, float max, bool& isClamped){
        if(val < min)
        {
            isClamped = true;
            return min;
        }
        else if(val > max)
        {
            isClamped = true;
            return max;
        }
        else
        {
            isClamped = false;
            return val;
        }
    };
    
    bool xClamped = false;
    bool yClamped = false;
    //(player->tileDelta / player->tileSize);
    
    lastCamUpdate += deltaTime;
    //std::cout << "cam " << lastCamUpdate << "\n";
    float updateIts = 0;
    if(lastCamUpdate >= 0.00694f)
    {
        updateIts = glm::floor(lastCamUpdate / 0.00694f);
        lastCamPos = glm::vec2(cam.position.x, cam.position.y);
        for(int x=0; x<updateIts; x++)
        {
            //camera shaking
            if(shaking && shakeTime < 1.0f)
            {
                float cx = cam.position.x;
                float cy = cam.position.y;
                float delta = 0.5f*glm::cos(shakeTime * glm::two_pi<float>());
                
                cam.setPosition(glm::vec3(cx, cy + delta, cam.position.z));
                shakeTime += 0.02082f;
            }else if(shaking && shakeTime >= 1.0f)
            {
                shaking = false;
                shakeTime = 0.0f;
            }
            
            float destX = map->resettable ? (map->width/2 - 2) * 17.0f : player->pos.x;
            float destY = map->resettable ? (map->height/2 - 1) * 17.0f : player->pos.y;
            float newCamX = glm::mix(cam.position.x, camX + destX, glm::smoothstep(0.0f, 1.0f, 0.15f));
            float newCamY = glm::mix(cam.position.y, camY + destY, glm::smoothstep(0.0f, 1.0f, 0.15f));
            
            newCamX = (map->width < 30 ? newCamX : 
                       boolClamp(newCamX, map->pos.x + 153.0f + camX, map->pos.x + 17.0f*map->width - 168.0f + camX, xClamped));
            
            newCamY = (map->height < 16 ? newCamY : 
                       boolClamp(newCamY, map->pos.y + 85.0f + camY, map->pos.y + 17.0f*map->height - 95.0f + camY, yClamped));
            
            glm::vec2 camDir = glm::vec2(player->pos.x - cam.position.x, player->pos.y - cam.position.y);
            //camDir = glm::normalize(camDir);
            
            cam.setPosition(glm::vec3(newCamX, newCamY, camZ));
        }
        
        //cam.setPosition(glm::vec3(glm::floor(cam.position.x), glm::floor(cam.position.y), camZ));
        
        lastCamUpdate -= (updateIts * 0.00694f);
        noCamUpdateFrames = 1.0f;
        updateSnow = true;
    }else
    {
        updateSnow = false;
        noCamUpdateFrames++;
    }
    
    
	//check tile for event and play it
	if (!curEvent && map != nullptr)
	{
		TileEventData* e = map->getTileEventDataPtrWorldPos(player->worldPos.x, player->worldPos.y);
		if (e != nullptr && curEvent != e && e->id > 0 && e->active)
		{
            bool hasReqs = true;
            std::vector<int> reqEvents = eventMachine.eventReqs[e->id - 1];
            for(int i=0; i < reqEvents.size(); i++)
            {
                int req = reqEvents[i];
                if(!completedEvents[req])
                {
                    hasReqs = false;
                    break;
                }
            }
            
            if(!e->limited || hasReqs)
            {
                lastEvent = e->id;
                curEvent = e;
            }
            
		}else if(e != nullptr && e->id <= lastEvent && e->limited)
        {
            e->active = false;
        }
	}
	if (curEvent && curEvent->id > 0 && curEvent->active)
	{
		bool isGoing = eventMachine.runEvent(*this, curEvent->id, deltaTime);
		if (!isGoing)
		{
			if (curEvent->limited) curEvent->active = false;
            completedEvents[curEvent->id - 1] = true;
            curEvent = nullptr;
        }
	}
	
	checkForPortal();
	
	if (!isLoading && !player->isMoving && inputHandler->isRDown && map->resettable)
	{
		size_t len = map->mapFile.length();
		resetName = new char[len + 1];
		memcpy(resetName, map->mapFile.c_str(), len);
		resetName[len] = '\0';
        
		PortalData d{ resetName, map->resetPos, true };
		loadMapFromPortalData(d);
        
		isResetting = true;
	}
    
	playerWorldPos = player->worldPos;
    
	GLfloat mapTime = glm::mod(time/2.0f, 2.0f);
	ShaderProgram* programMap = &assMan->shaderPrograms[SHADER_MAP];
	programMap->use();
	programMap->setFloat("time", (GLfloat)mapTime);
	programMap->setFloat("timeTrue", (GLfloat)time);
    
	//draw world (bottom layer)
	model = glm::mat4(1.0f);
	mvpMap = proj * cam.getViewMatrix() * model;
	programMap->setMat4("transform", &mvpMap);
	mapRenderer.draw(*programMap);
    
    //draw particles
    ShaderProgram* programInstance = &assMan->shaderPrograms[SHADER_DYNAMIC];
	part.updateParticles(deltaTime > 0.017f ? 0.017f : deltaTime);
	mapRenderer.setupMeshParticle(part.posi, part.uv, part.sizes, part.times, part.colors, part.count);
    
	ShaderProgram* programParticle = &assMan->shaderPrograms[SHADER_PARTICLE];
	programParticle->use();
	programParticle->setFloat("time", part.particles[0].timeAlive[0]);
	programParticle->setMat4("transform", &mvpMap);
    
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mapRenderer.meshParticle.drawInstanced(*programInstance, part.count);
	//glDisable(GL_BLEND);
    
	//draw dynamic tiles
	programInstance->use();
	programInstance->setFloat("time", player->timeGlow);
	programInstance->setMat4("transform", &mvpMap);
	mapRenderer.drawDynamic(*programInstance);
    
	//draw player and NPCS in correct order
	Player* pTmp = player;
	std::vector<NPC>& npcsTmp = npcs;
    
	//define sort function that sorts characters back to front (based on y world position)
	auto sortChars = [&pTmp, &npcsTmp](const int a, const int b) {
		float yA = 0.0f;
		float yB = 0.0f;
        
		if (a == -1)
		{
			yA = pTmp->pos.y;
		} else {
			yA = npcsTmp[a].second.pos.y;
		}
        
		if (b == -1)
		{
			yB = pTmp->pos.y;
		} else {
			yB = npcsTmp[b].second.pos.y;
		}
        
		return yA < yB;
	};
    
	//render function for characters
	auto renderChar = [](QuadAnimated* const q, const Player& p, const glm::mat4& proj, const glm::mat4& cam) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), p.pos);
		glm::mat4 mvpNpc = proj * cam * model;
		q->program.use();
		q->program.setMat4("transform", &mvpNpc);
		q->draw(p.playerAnimTime);
	};
    
	//Create vector with all NPC indices + player index (-1)
	if (charInd.size() != npcs.size() + 1)
	{
		charInd = { -1 };
		int ind = 0;
		for (auto it = npcs.begin(); it != npcs.end(); it++)
		{
			charInd.push_back(0);
		}
	}
    
	//call sorting function
	std::sort(charInd.begin(), charInd.end(), sortChars);
    
	//render characters in sorted order
	for (auto it = charInd.begin(); it != charInd.end(); it++)
	{
        int i = *it;
        QuadAnimated* q = i < 0 ? nullptr : npcs[i].first;
        Player& p = i < 0 ? *player : npcs[i].second;
        
        if(player->isAttacking && player->isAlive)
        {
            if(player->curDir == Dir::RIGHT && player->worldPos == p.worldPos + glm::vec2(-1, 0))
            {
                npcs[i].alive = false;
            }
            else if(player->curDir == Dir::LEFT && player->worldPos == p.worldPos + glm::vec2(1, 0))
            {
                npcs[i].alive = false;
            }
            else if(player->curDir == Dir::UP && player->worldPos == p.worldPos + glm::vec2(0, 1))
            {
                npcs[i].alive = false;
            }
            else if(player->curDir == Dir::DOWN && player->worldPos == p.worldPos + glm::vec2(0, -1))
            {
                npcs[i].alive = false;
            }
        }
        
		if(*it == -1) renderChar(&quadAnim, *player, proj, cam.getViewMatrix());
		else 
        {
            //This is for enemy npc type updates (idle animation)
			if(npcs[i].alive && npcs[i].id == ENEMY_ID_LOOKER)
            {
                glm::vec2 npcLocPos = p.worldPos - glm::vec2(map->worldPos.x, map->worldPos.y);
                
                glm::vec2 normDir = glm::normalize(player->worldPos - p.worldPos);
                float d = glm::dot(normDir, glm::vec2(1,0));
                
                if(d > 0.5f)
                {
                    p.quadAnim.getAnimationController()->setState("w_right");
                    
                    if(player->worldPos == p.worldPos + glm::vec2(1, 0) && player->isAlive)
                    {
                        std::cout << "attacking player right\n";
                        killPlayer();
                    }
                }
                else if(d <= 0.5f && d >= -0.5f && player->worldPos.y < p.worldPos.y)
                {
                    p.quadAnim.getAnimationController()->setState("w_up");
                    
                    if(player->worldPos == p.worldPos + glm::vec2(0, -1) && player->isAlive)
                    {
                        std::cout << "attacking player up\n";
                        killPlayer();
                    }
                }
                else if(d < -0.5f)
                {
                    p.quadAnim.getAnimationController()->setState("w_left");
                    
                    if(player->worldPos == p.worldPos + glm::vec2(-1, 0) && player->isAlive)
                    {
                        std::cout << "attacking player left\n";
                        killPlayer();
                    }
                }
                else if(d <= 0.5f && d >= -0.5f && player->worldPos.y >= p.worldPos.y)
                {
                    p.quadAnim.getAnimationController()->setState("w_down");
                    
                    if(player->worldPos == p.worldPos + glm::vec2(0, 1) && player->isAlive)
                    {
                        std::cout << "attacking player down\n";
                        killPlayer();
                    }
                }
                
                size_t npcP = npcLocPos.y * map->width + npcLocPos.x;
                map->staticData1[npcP].id = 250;
                p.playerAnimTime += deltaTime;
            }
            else if(npcs[i].alive && npcs[i].id == ENEMY_ID_STATUE)
            {
                WorldEnums::WRL_DIR npcDir = p.curDir;
                
                if(npcDir == Dir::RIGHT && player->worldPos == p.worldPos + glm::vec2(1, 0) && player->isAlive)
                {
                    std::cout << "attacking player right\n";
                    killPlayer();
                }
                else if(npcDir == Dir::LEFT && player->worldPos == p.worldPos + glm::vec2(-1, 0) && player->isAlive)
                {
                    std::cout << "attacking player left\n";
                    killPlayer();
                }
                else if(npcDir == Dir::UP && player->worldPos == p.worldPos + glm::vec2(0, -1) && player->isAlive)
                {
                    std::cout << "attacking player up\n";
                    killPlayer();
                }
                else if(npcDir == Dir::DOWN && player->worldPos == p.worldPos + glm::vec2(0, 1) && player->isAlive)
                {
                    std::cout << "attacking player down\n";
                    killPlayer();
                }
            }
            
            if(npcs[i].alive) renderChar(q, p, proj, cam.getViewMatrix());
		} 
	}
    
	//draw world (top layer)
	programMap->use();
	programMap->setMat4("transform", &mvpMap);
	mapRenderer.drawTop(*programMap);
    
    //draw death screen
    if(!player->isAlive)
    {
        //draw soul sprite
        glm::mat4 sprite = glm::translate(glm::mat4(1.0f), player->pos - glm::vec3(8.0f, soulTime*12.0f, 0.0f));
		glm::mat4 mvpSprite = proj * cam.getViewMatrix() * sprite;
        
        soulTime += deltaTime;
        
        ShaderProgram& spriteProg = assMan->shaderPrograms[SHADER_SPRITE];
        spriteProg.use();
        spriteProg.setFloat("time", glm::mod(soulTime, 2.0f));
        spriteProg.setVec4("color", glm::vec4(1,1,1,0.5f));
		spriteProg.setMat4("transform", &mvpSprite);
        deathSpriteQuad.draw(spriteProg);
        
        /*ShaderProgram& programFont = assMan->shaderPrograms[SHADER_FONT];
        programFont.use();
        programFont.setFloat("time", 1000.0f);
        programFont.setVec3("color", glm::vec3(1, 1, 1));
        centerText.setTextUtf16(deathText, 8, false, 1.0f);
        centerText.draw(programFont);*/
        
        ShaderProgram& logoProg = assMan->shaderPrograms[SHADER_SCREEN_QUAD];
        logoProg.use();
        
        deathQuad.textures = { deathImgOut };
        deathQuad.draw(logoProg);
        deathQuad.textures = { deathImg };
        deathQuad.draw(logoProg);
    }
    
	//POST PROCESS PASS
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.0, 0.0, 0.0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	isFullScreen ? glViewport((displayMode.w / 2) - (SCREEN_WIDTH / 2), (displayMode.h / 2) - (SCREEN_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT)
        : glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
	ShaderProgram* screenProgram;
    
	if (map->isSnowing && map->isCave)
	{
		screenProgram = &assMan->shaderPrograms[SHADER_CAVE];
	}
	else if (map->isSnowing)
	{
		if(!fastSnow) screenProgram = &assMan->shaderPrograms[SHADER_SNOW];
		else screenProgram = &assMan->shaderPrograms[SHADER_FAST_SNOW];
    }
	else
	{
		screenProgram = &assMan->shaderPrograms[SHADER_NO_SNOW];
	}
    
	screenProgram->use();	
	screenProgram->setFloat("fadeTime", fadeTime);
	
	glm::vec2 playerVel(0.0f, 0.0f);
	glm::vec2 particleVel;
    
	/*if (player->isMoving && !isLoading)
	{
		if (player->curDir == WorldEnums::WRL_DIR::UP && !yClamped)
		{
			playerVel = glm::vec2(0.0f, 60.0f);
        }
        
		if (player->curDir == WorldEnums::WRL_DIR::DOWN && !yClamped)
		{
			playerVel = glm::vec2(0.0f, -60.0f);
        }
        
		if (player->curDir == WorldEnums::WRL_DIR::LEFT && !xClamped)
		{
			playerVel = glm::vec2(-60.0f, 0.0f);
        }
        
		if (player->curDir == WorldEnums::WRL_DIR::RIGHT && !xClamped)
		{
			playerVel = glm::vec2(60.0f, 0.0f);
        }
    }
    
	if (inputHandler->isShiftDown)
	{
		playerVel *= 1.5f;
	}*/
    
    //playerVel *= 60.0f;
    //playerVel /= noCamUpdateFrames;
    
	if (map->isSnowing && map->isCave)
	{
		particleVel = glm::vec2(0.0f, 1.0f / 9.0f);
	}
	else if (map->isSnowing)
	{
		particleVel = glm::vec2(1.0f / 12.0f, -1.0f / 6.0f) * 0.00694f;
	}
    
	if (map->isSnowing && updateSnow)
	{
        playerVel = glm::vec2(cam.position.x, cam.position.y) - lastCamPos;
        playerVel /= glm::vec2(480.0f, -270.0f);
        
		glm::vec2 relVelModifier = calculateRelVelModifier(playerVel/updateIts, particleVel);
		
        for(int i=0; i < updateIts; i++)
        {
            weatherTimeX += relVelModifier.x;
            weatherTimeY += relVelModifier.y;
        }
        
        
		if(!fastSnow) screenProgram->setFloat("time", (GLfloat)time);
		screenProgram->setFloat("timeY", (GLfloat)weatherTimeY);
		screenProgram->setFloat("timeX", (GLfloat)weatherTimeX);
	}
	
	if(map->isSnowing && !fastSnow) screenQuad.textures = { textureColorBuffer, textures["textureRandom"] };
	else if (map->isSnowing && fastSnow) screenQuad.textures = { textureColorBuffer, snowImg }; //TODO: Add snow texture here, make these all take in render targets. Needed to reduce artifacts!! Reset undo buffer on load of new level/refresh
	else if(!map->isSnowing) screenQuad.textures = { textureColorBuffer };
    
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	screenQuad.draw(*screenProgram);
    
	/*if (!savedSnow)
	{
		char* buffer = new char[1920 * 1080 * 4];
		memset(buffer, 0, 1920 * 1080 * 4);

		glReadPixels(0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		stbi_write_png("new_snow.png", 1920, 1080, 4, buffer, 1920 * 4);
		//stbi_write_jpg("poop.jpg", 1920, 1080, 3, buffer, 100);
		savedSnow = true;
		delete buffer;
	}*/
    
	diaBox->draw(assMan->shaderPrograms[SHADER_BG], assMan->shaderPrograms[SHADER_SCREEN_QUAD], assMan->shaderPrograms[SHADER_FONT], deltaTime);
} 

void MapScene::end()
{
	//clean up shaders and images
	cleanUpResources();
	delete map;
	delete player;
	delete diaBox;
	ended = true;
}

void MapScene::refreshMap()
{
	mapRenderer.refresh();
}