#pragma once
#include "MapScene.h"
#include "ScreenQuad.h"
#include "AssetManager.h"
#include "NatGui.h"

#include <SDL.h>

//test
#include "SDL_ttf.h"
#include <string>
#include <unordered_map>

class MenuScene : public Scene
{
    
    public:
    float time;
    float transTime;
    bool mouseDown;
    bool starting;
    bool settingsOpen;
    bool& running;
    AssetManager* assMan;
    ScreenQuad bgQuad;
    ScreenQuad logoQuad;
    ScreenQuad fadeQuad;
    SDL_DisplayMode displayMode;
    NatGui natGui;
    InputHandler* inputHandler;
    
    Scene** curScene;
    MapScene* mapScene;
    
    MenuScene(int width, int height, bool& running, AssetManager* assMan, SDL_DisplayMode displayMode, MusicManager& musMan,
              Scene** curScene, MapScene* mapScene);
    ~MenuScene();
    
    void start();
    void run(float deltaTime);
    void end();
    
};

