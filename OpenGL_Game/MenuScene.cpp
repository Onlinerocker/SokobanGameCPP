#include "MenuScene.h"

#include <GL\glew.h>
#include <iostream>

//test
#include <fstream>

MenuScene::MenuScene(int width, int height, bool& running, AssetManager* assMan, SDL_DisplayMode displayMode, MusicManager& musMan, Scene** curScene, MapScene* mapScene) : 
logoQuad(glm::vec2(410,550), glm::vec2(1100.0, 423.0), 1920, 1080), 
natGui(assMan->textures[ASSET_MAIN_MENU].id, width, height, 1920, 1080, musMan),
running(running)
{
    isFullScreen = false;
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    
    this->assMan = assMan;
    this->displayMode = displayMode;
    this->curScene = curScene;
    this->mapScene = mapScene;
    
    ended = false;
    starting = false;
    settingsOpen = false;
    mouseDown = false;
    type = 2;
    time = 0.0f;
    transTime = 0.0f;
    
    inputHandler = InputHandler::GetInputHandler();
}

MenuScene::~MenuScene()
{
}

void MenuScene::run(float deltaTime)
{
    if (ended) return;
    
    int mx, my;
    int guiClick = SDL_GetMouseState(&mx, &my);
    
    if(guiClick == 1 && !mouseDown)
    {
        mouseDown = true;
    }
    
    int mCorX = isFullScreen ? (displayMode.w / 2) - (SCREEN_WIDTH / 2) : 0;
    int mCorY = isFullScreen ? (displayMode.h / 2) - (SCREEN_HEIGHT / 2) : 0;
    
    mx -= mCorX;
    my -= mCorY;
    mx = glm::clamp(mx, 0, SCREEN_WIDTH);
    my = glm::clamp(my, 0, SCREEN_HEIGHT);
    
    time += deltaTime;
    
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);
    glDisable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    isFullScreen ? glViewport((displayMode.w / 2) - (SCREEN_WIDTH / 2), (displayMode.h / 2) - (SCREEN_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT)
        : glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    assMan->shaderPrograms[13].use();
    assMan->shaderPrograms[13].setFloat("time", time);
    bgQuad.draw(assMan->shaderPrograms[13]);
    
    ShaderProgram& logoProg = assMan->shaderPrograms[SHADER_SCREEN_QUAD];
    logoProg.use();
    logoQuad.textures = { assMan->textures[ASSET_LOGO].id };
    logoQuad.draw(logoProg);
    
    int kbInd = 0;
    natGui.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 2, deltaTime);
    if(!settingsOpen && !starting)
    {
        natGui.captureMouseInput(glm::vec2(mx, my), guiClick < 1 && mouseDown, mouseDown);
        natGui.captureKbInput({ inputHandler->isWDown, inputHandler->isSDown, inputHandler->isADown, 
                                  inputHandler->isDDown, inputHandler->isEnterDown });
    }
    
    if(natGui.button(glm::vec2(725, 350), glm::vec2(410, 90), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), 0, kbInd++) && !starting)
    {
        starting = true;
        transTime = 1.0f;
    }
    
    if(natGui.button(glm::vec2(725, 250), glm::vec2(410, 90), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), 1, kbInd++))
    {
        settingsOpen = true;
    }
    
    if(natGui.button(glm::vec2(725, 150), glm::vec2(410, 90), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), 2, kbInd++))
    {
        running = false;
    }
    
    assMan->shaderPrograms[SHADER_NATGUI].use();
    glEnable(GL_FRAMEBUFFER_SRGB);
    natGui.draw(assMan->shaderPrograms[SHADER_NATGUI]);
    
    if(guiClick < 1 && mouseDown)
    {
        mouseDown = false;
    }
    
    if(starting)
    {
        transTime -= deltaTime/3.0f;
        
        if(transTime <= 0.0f)
        {
            *curScene = mapScene;
        }
    }
    else if(transTime < 1.0f)
    {
        transTime += deltaTime/3.0f;
        if(transTime > 1.0f) transTime = 1.0f;
    }
    
    ShaderProgram& csTransProgram = assMan->shaderPrograms[SHADER_CS_TRANS];
    csTransProgram.use();
    csTransProgram.setFloat("fadeTime", transTime);
    fadeQuad.draw(csTransProgram);
}

void MenuScene::start()
{
}

void MenuScene::end()
{
}