#pragma once

#include "Scene.h"
#include "Map.h"
#include "MapRenderer.h"
#include "Player.h"
#include "Camera.h"
#include "MusicManager.h"
#include "ShaderProgram.h"
#include "InputHandler.h"
#include "QuadAnimated.h"
#include "TrueTypeFont.h"
#include "DialogueBox.h"
#include "AssetManager.h"
#include "EventMachine.h"
#include "Particle.h"

#include "glm/glm.hpp"

#include <unordered_map>

#define ENEMY_ID_LOOKER 1
#define ENEMY_ID_STATUE 2

struct NPC
{
    QuadAnimated* first;
    Player second;
    int id = 0;
    bool alive = true;
    
    NPC& operator=(NPC other)
    {
        return *this;
    }
};

class MapScene : public Scene
{
	
    private:
	int doorFrame;
	float time, weatherTimeX, weatherTimeY;
    float doorParticleDelay;
    float doorParticleDelayInterval;
    float lastCamUpdate, noCamUpdateFrames;
	char* resetName;
	bool isResetting;
	bool savedSnow;
    bool updateSnow;
    bool shaking;
    float shakeTime;
    Uint16 deathText[32];
    
	PortalData newMapData;
    
	MapRenderer mapRenderer;
	QuadAnimated quadAnim;
	ScreenQuad screenQuad;
	SDL_DisplayMode displayMode;
	EventMachine eventMachine;
    
	MusicManager* musicManager;
	InputHandler* inputHandler;
    
	EventPtr eventPtr;
    
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
    
	glm::vec2 playerWorldPos;
    glm::vec2 lastCamPos;
    glm::vec2 lastSnowTime;
    
	std::vector<int> charInd;
    bool completedEvents[128];
    
	void cleanUpResources(); //delete all fonts
	void loadMapFromPortalData(const PortalData& portalData);
	bool checkForPortal();
	glm::vec2 calculateRelVelModifier(glm::vec2 velObj, glm::vec2 velParticle);
    
    public:
    float camX, camY, camZ;
    float fadeTime;
    float camTime;
    float soulTime;
    int lastEvent;
	bool isLoading;
	bool changeToCutscene;
    bool fastSnow;
    
    int wDeath, hDeath;
    GLuint deathImg;
    GLuint deathImgOut;
    GLuint frameBuffer, textureColorBuffer, rbo, snowImg;
    
	glm::mat4 mvpMap;
	DialogueBox* diaBox;
	Map* map;
	Player* player;
	AssetManager* assMan;
	Particle part;
	TileEventData* curEvent;
    Camera cam;
    FontRenderer centerText;
    FontRenderer centerTextOut;
    ScreenQuad deathQuad;
    Quad deathSpriteQuad;
    
	std::vector<NPC> npcs;
	std::unordered_map<const char*, GLuint> textures;
    
	MapScene(Map* map, GLuint mapTexture, MusicManager* musicManager, AssetManager* assMan, SDL_DisplayMode displayMode, int width, int height, float pixelWidth, float pixelHeight);
	~MapScene();
    
    void killPlayer();
	void start();
	void run(float deltaTime);
	void end();
	void refreshMap();
};

