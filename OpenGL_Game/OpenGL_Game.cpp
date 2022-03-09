// OpenGL_Game.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#define SCREEN_WIDTH 3440
//#define SCREEN_HEIGHT 1440

//#define GDBG

/*

On item GUI call
    supported resolutions
        3840x2160
        2560x1440
        1920x1080
        1280x720

    get position
    create vertices
    convert vertices screen pos to NDC
    push vertices (includes UV coords) to vector


Design (72 verts):

    [Resume]
    
    [x] Vsync
    [x] Fullscreen
    [<| 1920x1080 |>] Resolution

    [-------|----] Music Volume
    [----|-------] SFX Volume

    [Exit Game]

*/


#define DEBUG_BUILD /* I'm also in Map.h */

#include <iostream>
#include <fstream>

#include <SDL.h>
#include <GL\glew.h>
#include <SDL_opengl.h>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

#include "soloud.h"
#include "soloud_wav.h"

#include "SDL_ttf.h"

#ifdef DEBUG_BUILD
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#endif

#include "ShaderUtil.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "InputHandler.h"
#include "TextureUtil.h"
#include "Mesh.h"
#include "Quad.h"
#include "ScreenQuad.h"
#include "QuadAnimated.h"
#include "MusicManager.h"
#include "TrueTypeFont.h"
#include "FontRenderer.h"
#include "Player.h"
#include "DialogueBox.h"
#include "Map.h"
#include "MapRenderer.h"
#include "MapScene.h"
#include "SceneManager.h"
#include "NatGui.h"
#include "AssetManager.h"
#include "MenuScene.h"
#include "GameState.h"

int SCREEN_WIDTH, SCREEN_HEIGHT;
int mouseX, mouseY;
int mousePixelX, mousePixelY;
int mouseWorldX, mouseWorldY;

bool fastSnow;
bool paused;
bool escDown;

#ifdef DEBUG_BUILD
bool imGui;
#endif


InputHandler *inputHandler;
MusicManager* musicMan;

MapScene* mapScene;
Scene* curScene;

void OnEvent(SDL_Event* event, bool& running, float deltaTime)
{
    MapScene* ms = static_cast<MapScene*>(curScene);
    switch (event->type)
    {
        case SDL_WINDOWEVENT:
        switch (event->window.event)
        {
            case SDL_WINDOWEVENT_CLOSE:
            running = false;
            break;
        }
        
        break;
        
        case SDL_KEYDOWN:
        if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE && !escDown)
        {
            escDown = true;
            
            if(curScene->type < 2) paused = !paused;
            else paused = false;
            
            if(curScene->type == 2)
            {
                static_cast<MenuScene*>(curScene)->settingsOpen = false;
            }
            
            break;
        }
        if (event->key.keysym.scancode == SDL_SCANCODE_J)
        {
            if (curScene->type == 0)
            {
                MapScene* mapSc = static_cast<MapScene*>(curScene);
                
                if(mapSc->diaBox->enabled && !paused)
                {
                    mapSc->diaBox->nextDialogue();
                }
                else
                {
                    const TileInteractData* interactData = mapSc->map->getTileInteractDataWorldPos(mapSc->player->worldPos.x,
                                                                                                   mapSc->player->worldPos.y);
                    if(interactData && interactData->type >= 0)
                    {
                        mapSc->diaBox->dialoguePairs.clear();
                        mapSc->diaBox->dialogueNames.clear();
                        
                        std::string name(interactData->object.name);
                        
                        //TODO Build dialogue here, add function to dialogue builder class
                    }
                }
            }
            
            //musicMan->fadeToMusicLoop("Sounds/hotarumichi_loop/hotarumichi_intro.ogg", 
            //"Sounds/hotarumichi_loop/hotarumichi_loop.ogg", 2000);
            //break;
        }
        if (event->key.keysym.scancode == SDL_SCANCODE_Z && curScene->type == 0 && ms->map->resettable && !ms->player->isMoving && ms->map->stampNum > ms->map->stampBtm)
        {
            Map* m = ms->map;
            StateStamp& state = m->stateStamps[ms->map->stampNum - 1];
            Player* p = ms->player;
            
            memcpy(m->dynamicData, state.dynamicData, m->width * m->height * sizeof(DynamicData));
            memcpy(m->claimedData, state.claimedData, m->width * m->height * sizeof(bool));
            memcpy(m->staticData1, state.staticData1, m->width * m->height * sizeof(TileData));
            
            ms->map->stampNum -= 1;
            
            m->dynamicOffsets = state.dynamicOffsets;
            p->setPos(glm::vec3(state.playerFrom.x * 17.0f, state.playerFrom.y * 17.0f, 0.0f));
            p->curDir = state.directionFrom;
            
            switch (p->curDir)
            {
                
                case Dir::RIGHT:
                p->quadAnim.getAnimationController()->setState("w_right");
                break;
                case Dir::LEFT:
                p->quadAnim.getAnimationController()->setState("w_left");
                break;
                case Dir::UP:
                p->quadAnim.getAnimationController()->setState("w_up");
                break;
                case Dir::DOWN:
                p->quadAnim.getAnimationController()->setState("w_down");
                break;
                default:
                break;
                
            }
            
            ms->refreshMap();
        }
#ifdef DEBUG_BUILD
        if (event->key.keysym.scancode == SDL_SCANCODE_L)
        {
            imGui = !imGui;
            break;
        }
#endif
        inputHandler->onKeyDown(event->key);
        break;
        
        case SDL_KEYUP:
        if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE && escDown)
        {
            escDown = false;
        }
        inputHandler->onKeyUp(event->key);
        break;
        
        default:
        break;
        
    }
}

int main(int argc, char* argv[])
{
    // GLOBALS SETUP
    
    //TextureUtil::createBlob("Images/blob.dat", "Images/tiles.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/emelia.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/mech.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/random.jpg");
    //TextureUtil::createBlob("Images/blob.dat", "Images/noise.jpg");
    //TextureUtil::createBlob("Images/blob.dat", "Images/player_walk.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/ui_temp8.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/emelia_walk.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/color-logo.png");
    //TextureUtil::createBlob("Images/blob.dat", "Images/ui_menu.png");
    //return 0;
    
#ifdef DEBUG_BUILD
    imGui = true;
#endif
    
    GameState gameState{};
    gameState.playerX = 0;
    gameState.playerY = 0;
    gameState.lastEvent = -1;
    memset(gameState.mapName, 0, 128);
    
    paused = false;
    fastSnow = true;
    bool running = true;
    bool fullscreen = false;
    
    SCREEN_WIDTH = 1280;// 3440;
    SCREEN_HEIGHT = 720;
    
    int pixelX = 320;//512 for 2560
    int pixelY = 180;//288 for 1440
    
    musicMan = new MusicManager();
    
    inputHandler = InputHandler::GetInputHandler();
    //cam->rotate(0.0f, 90.0f);
    
    // SDL SETUP
    SDL_Window* window;
    SDL_GLContext context;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "[ERR] SDL could not init" << std::endl;
    
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    
    SCREEN_WIDTH = SCREEN_WIDTH > displayMode.w ? displayMode.w : SCREEN_WIDTH;
    SCREEN_HEIGHT = SCREEN_HEIGHT > displayMode.h ? displayMode.h : SCREEN_HEIGHT;
    
    std::cout << "Display DIM: " << displayMode.w << " x " << displayMode.h << "\n";
    //std::cout << "Game RES: " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << std::endl;
    
    window = SDL_CreateWindow("Isekai Tundra", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
    context = SDL_GL_CreateContext(window); //create GL context
    
    if (!context) std::cout << "[ERR] SDL Could not create GL context!" << std::endl;
    
    int vsync = 1;
    int swapSuc = SDL_GL_SetSwapInterval(vsync);
    if(swapSuc >= 0) std::cout << "VSync is " << (vsync ? "on\n" : "off\n");
    else std::cout << SDL_GetError() << std::endl;
    
    // GLEW SETUP
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "[ERR] GLEW: " << err << std::endl;
        return 1;
    }
    
    std::cout << "OpenGL v" << glGetString(GL_VERSION) << "\n";
    
    TTF_Init();
    
#ifdef DEBUG_BUILD
    /* START ImGUI */
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 330");
    /* END Dear ImGUI */
#endif
    
    double now, last, deltaTime, time;
    last = SDL_GetPerformanceCounter();
    time = 0;
    deltaTime = 0;
    now = 0;
    
    int width, height, uiWidth, uiHeight, ids;
    
    AssetManager assMan;
    AssetTextureData tilesData = assMan.textures[ASSET_TILES];
    width = tilesData.width;
    height = tilesData.height;
    
    AssetTextureData uiData = assMan.textures[ASSET_UI];
    uiWidth = uiData.width;
    uiHeight = uiData.height;
    
    GLuint mapTexture = tilesData.id;// .id;// TextureUtil::createTexture("Images/tiles.png", false, GL_NEAREST, &width, &height);
    GLuint uiTexture = uiData.id;
    
    ids = (width / 17) * (height / 17);
    
#ifdef DEBUG_BUILD
    int width1, height1;
    GLuint* mapTexturesDbg = new GLuint[ids];
    TextureUtil::createTexture("Images/tiles.png", false, GL_NEAREST, &width1, &height1, mapTexturesDbg, ids);
#endif
    
    TileManager tileManager(width, height, 17);
    Map* map = new Map(&tileManager, glm::vec3(-1, -1, 0), 17.0f, 10, 10);
    
    bool mapLoaded = map->loadMapFromFile("map_file1.natmap");
    
    //bool mapLoaded = map->loadMapFromFile("TEST_BOOP3.natmap");
    
    SceneManager sceneManager(&tileManager, mapTexture, musicMan, &assMan, displayMode, SCREEN_WIDTH, SCREEN_HEIGHT, pixelX, pixelY);
    
    mapScene = new MapScene(map, mapTexture, musicMan, &assMan, displayMode, SCREEN_WIDTH, SCREEN_HEIGHT, pixelX, pixelY);
    mapScene->isFullScreen = fullscreen;
    //mapScene->map->setTileEvent(6, 2, 1, true, true);
    mapScene->start();
    mapScene->player->setPos(glm::vec3(-17, 34, 0));
    mapScene->cam.setPosition(glm::vec3(mapScene->camX + mapScene->player->pos.x, 
                                        mapScene->camY + mapScene->player->pos.y, 
                                        mapScene->camZ));
    
    /* Test enemy */
    QuadAnimated quadTest(assMan.textures[ASSET_EMELIA_WALK].id, 1.0f, 4, glm::vec3(0.0f, -8.0f, 0.0f), glm::vec2(17.0f, 25.0f), glm::vec2(0.25f, 1.0f / 4.0f));
	Player pTest(glm::vec3(5.0f * 17.0f, 5.0f * 17.0f, 0), 17.0f, nullptr, quadTest);
	pTest.isNPC = true;
    NPC testEnemy{&quadTest, pTest, ENEMY_ID_STATUE};
    mapScene->npcs.push_back(testEnemy);
    
    Player& pl = mapScene->npcs[mapScene->npcs.size() - 1].second;
    
    pl.setupMoving(Dir::LEFT);
    //pl.quadAnim.getAnimationController()->setState("w_left");
    /* End test enemy */
    
    MenuScene menuScene(SCREEN_WIDTH, SCREEN_HEIGHT, running, &assMan, displayMode, *musicMan, &curScene, mapScene);
    
    curScene = &menuScene;//mapScene;//
    
    musicMan->volume = 0.0f;
    musicMan->volumeFx = 0.1f;
    musicMan->playMusicLoop("Sounds/hotarumichi_loop/hotarumichi_intro.ogg", 
                            "Sounds/hotarumichi_loop/hotarumichi_loop.ogg");
    //musicMan->playMusicLoop("Sounds/Take-a-Rest_loop/Take-a-Rest_intro.ogg",
    //"Sounds/Take-a-Rest_loop/Take-a-Rest_loop.ogg");
    
    /* Pause Menu BG Testing */
    ShaderProgram guiProg = assMan.shaderPrograms[SHADER_NATGUI];
    Shader guiVert("Shaders/ui.vert", GL_VERTEX_SHADER);
    Shader guiFrag("Shaders/natGui.frag", GL_FRAGMENT_SHADER);
    /*guiProg.attachShader(guiVert);
    guiProg.attachShader(guiFrag);
    guiProg.linkProgram();
    guiProg.use();
    guiProg.getAttribute("diffuse");*/
    
    ShaderProgram guiBgProg;
    Shader guiBgFrag("Shaders/txtBg.frag", GL_FRAGMENT_SHADER);
    guiBgProg.attachShader(guiVert);
    guiBgProg.attachShader(guiBgFrag);
    guiBgProg.linkProgram();
    
    Mesh meshBg;
    GLuint inds[6] = { 0, 1, 2, 0, 2, 3 };
    Vertex vBg{};
    
    vBg.vertex = glm::vec2(-1,-1);
    vBg.uv = glm::vec2(0);
    meshBg.vertices.push_back(vBg);
    
    vBg.vertex = glm::vec2(-1, 1);
    vBg.uv = glm::vec2(0, 1);
    meshBg.vertices.push_back(vBg);
    
    vBg.vertex = glm::vec2(1, 1);
    vBg.uv = glm::vec2(1, 1);
    meshBg.vertices.push_back(vBg);
    
    vBg.vertex = glm::vec2(1, -1);
    vBg.uv = glm::vec2(1, 0);
    meshBg.vertices.push_back(vBg);
    
    for (int x = 0; x < 6; x++)
    {
        meshBg.indices.push_back(inds[x]);
    }
    
    meshBg.setupMesh();
    
    int windowSize[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };
    float musVol = musicMan->volume;
    float fxVol = musicMan->volumeFx;
#ifdef DEBUG_BUILD
    bool staticTop = false;
    bool editMode = false;
    bool portalMode = false;
    bool copyMode = false;
    bool dynamicMode = false;
    bool doorMode = false;
    bool eventMode = false;
    bool clickCopyDown = false;
    bool isTileIDSolid = false;
    bool isMapSnowing = map->isSnowing;
    bool isCave = map->isCave;
    bool isEditColor = false;
    
    glm::vec3 static1Color(1);
    
    TileData staticClip[256];
    TileData staticTopClip[256];
    
    int curDoor = -1;
    int copyInd = 0;
    int tileID = 0;
    int tileIDSolid = 1;
    int openID = 0;
    int reqId = 0;
    int curLayer = 0;
    int reqPos[2] = { 0 };
    int mapSize[2] = { map->width, map->height };
    int copyTop[2] = { 0 };
    int copyBtm[2] = { 0 };
    int portalLoc[2] = { 0 };
    int portalDestLoc[2] = { 0 };
    int doorPos[2] = { 0 };
    int closedID = 0;
    int dir[3] = { 0, 0, 0 };
    
    size_t doorReqDel = 0;
    
    char mapName[128] = "map_file.natmap";
    char sceneName[128] = "scene.natsce";
    char portalMapName[128] = "map_file.natmap";
    char curPortalMapName[128] = "";
    const char* saveStatus = "--";
    const char* loadStatus = "--";
    
    float loadDelay = 0.0f;
    float saveDelay = 0.0f;
    float sprintSpeed = mapScene->player->sprintSpeed;
    
    TileEventData* curEventData = NULL;
    size_t eventInd = 0;
    
    SpringData* curSpringData = nullptr;
    int springLoc[2] = { 0, 0 };
    
    auto resetEditModes = [&staticTop, &portalMode, &copyMode, &dynamicMode, &doorMode, &eventMode](bool& val)
    {
        bool temp = val;
        if (temp)
        {
            staticTop = false;
            portalMode = false;
            copyMode = false;
            dynamicMode = false;
            doorMode = false;
            eventMode = false;
        }
        val = temp;
    };
    
    float boobTime = 0.0f;
    Mesh editMesh;
    Vertex v[4];
    GLuint i[6] = { 0, 1, 2, 0, 2, 3 };
    float tileSize = 17.0f;
    glm::vec2 uvMin(0, 0);
    glm::vec2 uvMax(1, 1);
    glm::mat4 editProj(1.0f);
    
    Shader editVert("Shaders/vert.vert", GL_VERTEX_SHADER);
    Shader editFrag("Shaders/editFrag.frag", GL_FRAGMENT_SHADER);
    ShaderProgram editProgram;
    editProgram.attachShader(editVert);
    editProgram.attachShader(editFrag);
    editProgram.linkProgram();
    editProgram.use();
    editProgram.getAttribute("transform");
    editProgram.getAttribute("border");
    editProgram.getAttribute("color");
    editProgram.getAttribute("alpha");
    
    float mouseWorldXPixel = 0;
    float mouseWorldYPixel = 0;
    
    v[0].vertex = glm::vec3(0, 0, 0);
    v[0].uv = uvMin;
    
    v[1].vertex = glm::vec3(mouseWorldXPixel, mouseWorldYPixel + tileSize, 0);
    v[1].uv = glm::vec2(uvMin.x, uvMax.y);
    
    v[2].vertex = glm::vec3(mouseWorldXPixel + tileSize, v[1].vertex.y, 0);
    v[2].uv = uvMax;
    
    v[3].vertex = glm::vec3(v[2].vertex.x, mouseWorldYPixel, 0);
    v[3].uv = glm::vec2(uvMax.x, uvMin.y);
    
    for (int z = 0; z < 4; z++) editMesh.vertices.push_back(v[z]);
    for (int z = 0; z < 6; z++) editMesh.indices.push_back(i[z]);
    
    editMesh.setupMesh();
    
#endif
    
    int mouseCorrectionX = fullscreen ? (displayMode.w / 2) - (SCREEN_WIDTH / 2) : 0;
    int mouseCorrectionY = fullscreen ? (displayMode.h / 2) - (SCREEN_HEIGHT / 2) : 0;
    
    NatGui natGui(uiTexture, SCREEN_WIDTH, SCREEN_HEIGHT, 1920, 1080, *musicMan);
    bool guiClickDown = false;
    
    while (running && mapLoaded)
    {
        MapScene* curMapScene = curScene->type == 0 ? static_cast<MapScene*>(curScene) : nullptr;
        MenuScene* curMenuScene = curScene->type == 2 ? static_cast<MenuScene*>(curScene) : nullptr;
        
        musicMan->run();
        
        SDL_Event event;
        
        now = SDL_GetPerformanceCounter();
        deltaTime = (now - last) / (double)SDL_GetPerformanceFrequency();
        time += deltaTime;
        last = SDL_GetPerformanceCounter();
        
        int clicked = 0;
        
        while (SDL_PollEvent(&event))
        {
            clicked = SDL_GetMouseState(&mouseX, &mouseY);
            
#ifdef DEBUG_BUILD
            ImGui_ImplSDL2_ProcessEvent(&event);
            clicked = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) ? 0 : clicked;
            clicked = ImGui::GetIO().WantCaptureKeyboard ? 0 : clicked;
            
            if (curScene->type == 0 && !paused)
            {
                Map* map = mapScene->map;
                
                if (clicked && editMode && !copyMode)
                {
                    glm::vec2 p = glm::vec2(mouseWorldX, mouseWorldY) - glm::vec2(map->pos.x / 17, map->pos.y / 17);
                    
                    if (dynamicMode)
                    {
                        map->setTileDynamic(p.x, p.y, tileID);
                    }
                    else if (portalMode)
                    {
                        bool hasPortal = false;
                        portalLoc[0] = static_cast<int>(mouseWorldX);
                        portalLoc[1] = static_cast<int>(mouseWorldY);
                        PortalData pd = map->getPortalAtWorldPos(portalLoc[0], portalLoc[1], &hasPortal);
                        
                        if (!hasPortal)
                        {
                            map->setPortalAtWorldPos(portalLoc[0], portalLoc[1], 0, 0, "");
                        }
                        else
                        {
                            strcpy(curPortalMapName, pd.destinationMap);
                        }
                    }
                    else if (doorMode)
                    {
                        bool isDoor = false;
                        int x = 0;
                        for (auto it = map->doors.begin(); it != map->doors.end(); it++)
                        {
                            size_t p = it->position;
                            int doorX = p % map->width;
                            int doorY = p / map->width;
                            doorX += map->worldPos.x;
                            doorY += map->worldPos.y;
                            
                            if (clicked <= 2 && doorX == mouseWorldX && doorY == mouseWorldY)
                            {
                                doorPos[0] = doorX;
                                doorPos[1] = doorY;
                                curDoor = x;
                                isDoor = true;
                                break;
                            }
                            else if (clicked > 2 && doorX == mouseWorldX && doorY == mouseWorldY)
                            {
                                map->staticData[it->position].id = it->openId;
                                map->doors.erase(it);
                                isDoor = true;
                                break;
                            }
                            
                            x++;
                        }
                        
                        if (!isDoor)
                        {
                            int mouseLocX = mouseWorldX - map->worldPos.x;
                            int mouseLocY = mouseWorldY - map->worldPos.y;
                            if (mouseLocX >= 0 && mouseLocY >= 0)
                            {
                                if (clicked <= 2)
                                {
                                    DoorData door;
                                    door.position = mouseLocY * map->width + mouseLocX;
                                    door.isOpen = false;
                                    map->doors.push_back(door);
                                }
                            }
                        }
                    }
                    else if (eventMode && p.x < map->width && p.x >= 0 && p.y < map->height && p.y >= 0)
                    {
                        size_t i = map->width * p.y + p.x;
                        TileEventData* e = &map->eventData[i];
                        
                        if (e->id < 1) e->id = 1;
                        
                        curEventData = e;
                        eventInd = i;
                    }
                    else if (!staticTop)
                    {
                        if(isEditColor && curLayer == 1)
                        {
                            size_t loc = p.y * map->width + p.x;
                            map->staticData1Colors[loc] = static1Color;
                        }
                        else if(isEditColor && curLayer == 0)
                        {
                            size_t loc = p.y * map->width + p.x;
                            map->staticDataColors[loc] = static1Color;
                        }
                        else
                        {
                            map->setTileStatic(p.x, p.y, tileID, curLayer);
                        }
                    }
                    else map->setTileStaticTop(p.x, p.y, tileID);
                    
                    mapScene->refreshMap();
                }
                else if (clicked == 1 && editMode && copyMode)
                {
                    //paste clip board
                    int clipW = copyBtm[0] - copyTop[0] + 1;
                    int clipH = copyBtm[1] - copyTop[1] + 1;
                    
                    int mapX = map->pos.x / 17;
                    int mapY = map->pos.y / 17;
                    glm::vec2 pMouse = glm::vec2(mouseWorldX, mouseWorldY)
                        - glm::vec2(mapX, mapY);
                    
                    if (pMouse.x >= mapX && pMouse.y >= mapY && pMouse.x < mapX + map->width && pMouse.y < mapY + map->height)
                    {
                        for (int y = 0; y < clipH; y++)
                        {
                            for (int x = 0; x < clipW; x++)
                            {
                                glm::vec2 pPaste = pMouse + glm::vec2(x, y);
                                
                                int index = (y * clipW) + x;
                                
                                map->setTileStatic(pPaste.x, pPaste.y, staticClip[index].id, 0);
                                map->setTileStaticTop(pPaste.x, pPaste.y, staticTopClip[index].id);
                            }
                        }
                        mapScene->refreshMap();
                    }
                    else
                    {
                        std::cout << "[ERR] Copy error: mouse_p (" << pMouse.x << ", " << pMouse.y << ") == map_p (" << mapX << ", " << mapY << ")" << std::endl;
                    }
                    
                    
                }
                else if (clicked > 2 && editMode && copyMode)
                {
                    clickCopyDown = true;
                }
                
                if (clickCopyDown && clicked <= 2)
                {
                    clickCopyDown = false;
                    if (copyInd == 0)
                    {
                        copyTop[0] = mouseWorldX;
                        copyTop[1] = mouseWorldY;
                    }
                    else
                    {
                        copyBtm[0] = mouseWorldX;
                        copyBtm[1] = mouseWorldY;
                        //copy everything to clip board
                        int clipW = copyBtm[0] - copyTop[0] + 1;
                        int clipH = copyBtm[1] - copyTop[1] + 1;
                        
                        for (int y = 0; y < clipH; y++)
                        {
                            for (int x = 0; x < clipW; x++)
                            {
                                glm::vec2 p = glm::vec2(copyTop[0] + x, copyTop[1] + y)
                                    - glm::vec2(map->pos.x / 17, map->pos.y / 17);
                                int index = (y * clipW) + x;
                                
                                if (index > 255) break;
                                staticClip[index] = map->getTileStatic(p.x, p.y, 0);
                                staticTopClip[index] = map->getTileStaticTop(p.x, p.y);
                            }
                        }
                    }
                    
                    copyInd++;
                    copyInd %= 2;
                }
            }
            
#endif
            
            mouseX -= mouseCorrectionX;
            mouseY -= mouseCorrectionY;
            mouseX = glm::clamp(mouseX, 0, SCREEN_WIDTH);
            mouseY = glm::clamp(mouseY, 0, SCREEN_HEIGHT);
            
            float xPer = static_cast<float>(mouseX) / static_cast<float>(SCREEN_WIDTH);
            float yPer = static_cast<float>(mouseY) / static_cast<float>(SCREEN_HEIGHT);
            mousePixelX = xPer * static_cast<float>(pixelX);
            mousePixelY = yPer * static_cast<float>(pixelY);
            
            OnEvent(&event, running, deltaTime);
        }
        
#ifdef DEBUG_BUILD
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        
        ImGui::NewFrame();
        
        //Map editor settings
        if(curScene->type == 0 && !paused)
        {
            Map* map = mapScene->map;
            
            if (map->isSnowing != isMapSnowing) isMapSnowing = map->isSnowing;
            
            ImGui::Begin("Map Editor");                        // Create a window called "Hello, world!" and append into it.
            
            ImGui::Checkbox("Edit Mode", &editMode);
            if (ImGui::Checkbox("Static Top", &staticTop)) resetEditModes(staticTop);
            if (ImGui::Checkbox("Copy Mode", &copyMode)) resetEditModes(copyMode);
            if (ImGui::Checkbox("Dynamic Mode", &dynamicMode)) resetEditModes(dynamicMode);
            if (ImGui::Checkbox("Edit Portals", &portalMode)) resetEditModes(portalMode);
            if (ImGui::Checkbox("Edit Doors", &doorMode)) resetEditModes(doorMode);
            if (ImGui::Checkbox("Edit Events", &eventMode)) resetEditModes(eventMode);
            
            if (ImGui::Checkbox("Is Snowing", &isMapSnowing))
            {
                map->isSnowing = isMapSnowing;
            }
            
            ImGui::Checkbox("Fast Snow", &fastSnow);
            
            if (ImGui::Checkbox("Is Cave", &isCave))
            {
                map->isCave = isCave;
            }
            
            ImGui::Checkbox("Resettable", &map->resettable);
            
            ImGui::InputInt2("Copy Top", copyTop);
            ImGui::InputInt2("Copy Btm", copyBtm);
            
            ImGui::InputInt("Tile ID", &tileID);
            
            if (map->justLoaded)
            {
                strcpy(mapName, map->getMapFileName());
                map->justLoaded = false;
            }
            ImGui::InputText("File Name", mapName, 128);
            
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 300);
            ImGui::InputInt2("Map Size", mapSize);
            
            ImGui::InputFloat2("Reset Pos", reinterpret_cast<float*>(&map->resetPos));
            
            ImGui::NextColumn();
            if (ImGui::Button("Set Map Size", ImVec2(100, 25)))
            {
                map->setWidthHeight(mapSize[0], mapSize[1]);
                mapScene->refreshMap();
                mapScene->curEvent = nullptr;
            }
            
            ImGui::NextColumn();
            
            if (ImGui::Button("Save Map", ImVec2(100, 25)))
            {   
                ImGui::OpenPopup("Save Map");
                //map->isSnowing = isMapSnowing;
            }
            
            if (ImGui::BeginPopup("Save Map"))
            {
                ImGui::Text("Are you sure you want to save?");
                if (ImGui::Button("Yes"))
                {
                    if (map->saveMapToFile(mapName))
                    {
                        saveStatus = "Map saved!";
                        saveDelay = 0.0f;
                    }
                    else
                    {
                        saveStatus = "Could not save map";
                        saveDelay = 0.0f;
                    }
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("No"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            
            
            
            saveDelay += deltaTime;
            if (saveDelay >= 15.0f) saveStatus = "--";
            
            ImGui::NextColumn();
            ImGui::Text(saveStatus);
            
            ImGui::NextColumn();
            if (ImGui::Button("Load Map", ImVec2(100, 25)))
            {
                if (map->loadMapFromFile(mapName))
                {
                    mapScene->npcs.clear();
                    mapScene->curEvent = nullptr;
                    mapScene->refreshMap();
                    loadStatus = "Map loaded!";
                    loadDelay = 0.0f;
                    
                    mapSize[0] = map->width;
                    mapSize[1] = map->height;
                    isMapSnowing = map->isSnowing;
                }
                else
                {
                    loadStatus = "Could not load map";
                    loadDelay = 0.0f;
                    
                }
                
            }
            
            loadDelay += deltaTime;
            if (loadDelay >= 15.0f) loadStatus = "--";
            
            ImGui::InputInt("Btm Layer", &curLayer);
            
            ImGui::NextColumn();
            ImGui::Text(loadStatus);
            
            ImGui::Columns(5);
            
            ImVec4 tint(1, 1, 1, 1);
            
            if (ImGui::ImageButton((void*)mapTexture, ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), 5, ImVec4(0, 0, 0, 1), ImVec4(0,0,0,1)))
            {
                tileID = 0;
            }
            ImGui::NextColumn();
            
            for (unsigned int id = 1; id <= ids; id++)
            {
                //std::cout << "yo2" << std::endl;
                glm::vec4 tileUv = tileManager.getTileUV(id);
                
                if (tileID == id) tint = ImVec4(0, 0.5, 0.5, 1);
                else tint = ImVec4(1, 1, 1, 1);
                
                if (ImGui::ImageButton(reinterpret_cast<void*>(mapTexturesDbg[id-1]), ImVec2(50, 50), ImVec2(tileUv.x, tileUv.y), ImVec2(tileUv.z, tileUv.w), 5, ImVec4(0, 0, 0, 1), tint))
                {
                    tileID = id;
                }
                ImGui::NextColumn();
            }
            
            ImGui::End();
            
            if(editMode)
            {
                ImGui::Begin("Static Color Picker");
                ImGui::Checkbox("Edit Color", &isEditColor);
                ImGui::ColorPicker3("Color", &static1Color.x);
                ImGui::End();
            }
            
            /* Solid Tiles */
            ImGui::Begin("Solid Tiles");
            
            if (ImGui::BeginListBox(""))
            {
                for (unsigned int i = 0; i < 255; i++)
                {
                    ImGui::Text("%d %s", static_cast<int>(i), tileManager.isTileSolid(i) ? "SOLID" : "NOT SOLID");
                }
                
                ImGui::EndListBox();
            } 
            
            if (ImGui::InputInt("ID", &tileIDSolid))
            {
                tileIDSolid = glm::clamp(tileIDSolid, 0, 255);
                isTileIDSolid = tileManager.solids[tileIDSolid];
            }
            
            ImGui::Checkbox("Is Solid", &isTileIDSolid);
            
            if (ImGui::Button("Save"))
            {
                tileManager.solids[tileIDSolid] = isTileIDSolid;
            }
            
            ImGui::End();
        }
        
        //Scene saving and loading
        {
            ImGui::Begin("Scene");
            
            if (ImGui::Button("Save Scene", ImVec2(100, 25)))
            {
                sceneManager.saveSceneToFile(sceneName, 0, curScene);
            }
            
            ImGui::InputText("Scene", sceneName, 128);
            
            if (ImGui::Button("Load Scene", ImVec2(100, 25)))
            {
                if (sceneManager.loadSceneFromFile(sceneName, sceneName))
                {
                    curScene->end();
                    curScene = sceneManager.getScene(sceneName).scene;
                    curScene->start();
                    
                    if (curScene->type == 0)
                    {
                        strcpy(mapName, curMapScene->map->getMapFileName());
                    }
                }
            }
            
            ImGui::End();
        }
        
        //Portal Info
        if(portalMode && curScene->type == 0)
        {
            ImGui::Begin("Portal Info");
            
            MapScene* mapScene = curMapScene;
            Map* map = mapScene->map;
            
            bool hasPortal = false;
            PortalData pd = map->getPortalAtWorldPos(portalLoc[0], portalLoc[1], &hasPortal);
            
            ImGui::Text("World Position: (%d, %d)", portalLoc[0], portalLoc[1]);
            ImGui::Text("Cur Destination Map: \"%s\"", curPortalMapName);
            ImGui::Text("Cur Destination Map Pos: (%.1f, %.1f)", pd.playerLoc.x, pd.playerLoc.y);
            ImGui::InputInt2("Map Destination Position: ", portalDestLoc);
            ImGui::InputText("New Destination Map", portalMapName, 128);
            
            if (ImGui::Button("Save Portal", ImVec2(100, 25)))
            {
                map->setPortalAtWorldPos(portalLoc[0], portalLoc[1], portalDestLoc[0], portalDestLoc[1], portalMapName);
            }
            
            if (ImGui::Button("Delete Portal", ImVec2(100, 25)))
            {
                map->deletePortalAtWorldPos(portalLoc[0], portalLoc[1]);
            }
            
            ImGui::End();
        }
        
        //Door info
        if (doorMode && curScene->type == 0 && curDoor >= 0)
        {
            ImGui::Begin("Door Info");
            ImGui::Text("World Position: (%d, %d)", doorPos[0], doorPos[1]);
            ImGui::Text("Cur Open ID: %d", map->doors[curDoor].openId);
            ImGui::Text("Cur Closed ID: %d", map->doors[curDoor].closedId);
            ImGui::Text("Direction: (%d, %d, %d)", map->doors[curDoor].dir[0], map->doors[curDoor].dir[1], map->doors[curDoor].dir[2]); 
            ImGui::InputInt("Open ID", &openID);
            ImGui::InputInt("Closed ID", &closedID);
            ImGui::InputInt3("Direction", dir);
            
            if (ImGui::Button("Save Door"))
            {
                map->doors[curDoor].openId = openID;
                map->doors[curDoor].closedId = closedID;
                for (int x = 0; x < 3; x++) map->doors[curDoor].dir[x] = dir[x];
            }
            
            if (ImGui::BeginListBox(""))
            {
                size_t x = 0;
                for (auto it = map->doors[curDoor].requirements.begin(); it != map->doors[curDoor].requirements.end() && map->doors[curDoor].requirements.size() > 0; it++)
                {
                    int reqY = it->first / map->width;
                    int reqX = it->first % map->width;
                    reqY += map->worldPos.y;
                    reqX += map->worldPos.x;
                    char buf[25];
                    sprintf(buf, "ID %d at (%d, %d)", static_cast<int>(it->second), reqX, reqY);
                    if (ImGui::Selectable(buf, doorReqDel == x))
                    {
                        doorReqDel = x;
                    }
                    x++;
                }
                
                ImGui::EndListBox();
            }
            
            ImGui::InputInt("Req ID", &reqId);
            ImGui::InputInt2("Req Pos", reqPos);
            
            if (ImGui::Button("Add Req.") && reqPos[0] >= map->worldPos.x && reqPos[1] >= map->worldPos.y && reqPos[0] < map->worldPos.x + map->width && reqPos[1] < map->worldPos.y + map->height)
            {
                reqPos[0] -= map->worldPos.x;
                reqPos[1] -= map->worldPos.y;
                map->doors[curDoor].requirements.push_back({ reqPos[1] * map->width + reqPos[0], reqId });
            }
            
            if (ImGui::Button("Delete Req."))
            {
                map->doors[curDoor].requirements.erase(map->doors[curDoor].requirements.begin() + doorReqDel);
            }
            
            ImGui::End();
        }
        
        //Debug info
        {
            glm::vec2 playerWorldPos = curScene->type == 0 ? curMapScene->player->worldPos : glm::vec2(0);
            
            ImGui::Begin("Debug");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Player World Position: (%.1f, %.1f)", playerWorldPos.x, playerWorldPos.y);
            ImGui::Text("Mouse Position: (%d, %d)", mouseX, mouseY);
            ImGui::Text("Mouse Pixel Position: (%d, %d)", mousePixelX, mousePixelY);
            
            int tileX = (mousePixelX + 7 - (pixelX / 2));
            int tileY = (mousePixelY - 17 - (pixelY / 2));
            
            ImGui::Text("Mouse Pixel From Player: (%d, %d)", tileX, tileY);
            
            int tileYPixel = (mousePixelY - (pixelY / 2));
            ImGui::Text("Mouse Pixel World: (%.1f, %.1f)", (playerWorldPos.x * 17.0f) + tileX, (playerWorldPos.y * 17.0f) + tileYPixel);
            
            if (tileX < 0) tileX -= 17;
            if (tileY > 0) tileY += 17;
            
            tileX /= 17;
            tileY /= 17;
            
            mouseWorldX = static_cast<int>(playerWorldPos.x) + tileX;
            mouseWorldY = static_cast<int>(playerWorldPos.y) + tileY;
            
            ImGui::Text("Mouse Screen Tile From Player: (%d, %d)", tileX, tileY);
            ImGui::Text("Mouse Tile World: (%d, %d)", mouseWorldX, mouseWorldY);
            
            ImGui::Checkbox("Ghost Mode", &mapScene->map->walkThrough);
            if (ImGui::InputFloat("Sprint Speed", &sprintSpeed))
            {
                mapScene->player->sprintSpeed = sprintSpeed;
            }
            
            ImGui::End();
        }
        
        //Event info
        if(curEventData)
        {
            ImGui::Begin("Event Info");
            
            if (ImGui::InputInt("ID", &curEventData->id))
            {}
            
            if (ImGui::Checkbox("Active", &curEventData->active))
            {}
            
            if (ImGui::Checkbox("Limited", &curEventData->limited))
            {}
            
            ImGui::End();
        }
        
        //Cutscene Spring info
        if(curSpringData)
        {
            ImGui::Begin("Cutscene Spring Info");
            
            ImGui::InputInt2("Position", springLoc);
            ImGui::InputFloat("Stiffness", &curSpringData->spring);
            ImGui::InputFloat("Dampening", &curSpringData->damp);
            
            ImGui::End();
        }
        
        //Window Info
        {
            ImGui::Begin("Game Settings");
            
            if (ImGui::Checkbox("Full Screen", &fullscreen))
            {
                curScene->isFullScreen = fullscreen;
                if (fullscreen) 
                {
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    //SDL_SetWindowSize(window, displayMode.w, displayMode.h);
                    
                    mouseCorrectionX = (displayMode.w / 2) - (SCREEN_WIDTH / 2);
                    mouseCorrectionY = (displayMode.h / 2) - (SCREEN_HEIGHT / 2);
                }
                else
                {
                    SDL_SetWindowFullscreen(window, 0);
                    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
                    mouseCorrectionX = 0;
                    mouseCorrectionY = 0;
                }
            }
            
            bool vsyncBool = static_cast<bool>(vsync);
            if (ImGui::Checkbox("VSync", &vsyncBool))
            {
                vsync = static_cast<int>(vsyncBool);
                swapSuc = SDL_GL_SetSwapInterval(vsync);
                
                if (swapSuc < 0) std::cout << SDL_GetError() << std::endl;
            }
            
            ImGui::InputInt2("Size", windowSize);
            if (ImGui::Button("Set Size"))
            {
                SCREEN_WIDTH = windowSize[0];
                SCREEN_HEIGHT = windowSize[1];
                
                curScene->SCREEN_WIDTH = windowSize[0];
                curScene->SCREEN_HEIGHT = windowSize[1];
                
                if (!fullscreen)
                {
                    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
                    mouseCorrectionX = 0;
                    mouseCorrectionY = 0;
                }
                else
                {
                    SDL_SetWindowSize(window, displayMode.w, displayMode.h);
                    mouseCorrectionX = (displayMode.w / 2) - (SCREEN_WIDTH / 2);
                    mouseCorrectionY = (displayMode.h / 2) - (SCREEN_HEIGHT / 2);
                }
                
                mapScene->start();
            }
            
            if (ImGui::DragFloat("Music Volume", &musVol, 0.01f, 0.0f, 1.0f))
            {
                musVol = glm::clamp(musVol, 0.0f, 1.0f);
                musicMan->setVolumeMusic(musVol);
            }
            
            ImGui::End();
        }
        
#endif
        
        if(curScene->type == 0 && curMapScene->fastSnow != fastSnow) curMapScene->fastSnow = fastSnow;
        curScene->run(paused ? 0 : deltaTime);
        
        if(curScene->type == 0 && curMapScene)
        {
            gameState.playerX = curMapScene->player->worldPos.x;
            gameState.playerY = curMapScene->player->worldPos.y;
            if(strcmp(gameState.mapName, curMapScene->map->mapFile.c_str()) != 0)
            {
                memcpy(gameState.mapName, curMapScene->map->mapFile.c_str(), curMapScene->map->mapFile.length());
                gameState.mapName[curMapScene->map->mapFile.length()] = '\0';
            }
        }
        
#ifdef DEBUG_BUILD
        
        if (editMode && curScene->type == 0)
        {
            MapScene* mapSc = curMapScene;
            Map* map = mapSc->map;
            
            mouseWorldXPixel = mouseWorldX * 17.0f;
            mouseWorldYPixel = mouseWorldY * 17.0f;
            
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            editProgram.use();
            
            /* Draw portals */
            if (portalMode)
            {
                editProgram.setFloat("border", 0.5f);
                glm::vec3 mapPos = map->pos;
                for (int y = 0; y < map->height; y++)
                {
                    for (int x = 0; x < map->width; x++)
                    {
                        bool hasPortal = false;
                        
                        float portalWorldXPixel = mapPos.x + (tileSize * x);
                        float portalWorldYPixel = mapPos.y + (tileSize * y);
                        
                        //std::cout << "portal " << portalWorldXPixel << ", " << portalWorldYPixel << std::endl;
                        
                        map->getPortalAtWorldPos(portalWorldXPixel / tileSize, portalWorldYPixel / tileSize, &hasPortal);
                        
                        if (hasPortal)
                        {
                            glm::vec3 c = (portalWorldXPixel / tileSize == portalLoc[0] && portalWorldYPixel / tileSize == portalLoc[1]) ?
                                glm::vec3(0.0f, 0.0f, 0.5f) : glm::vec3(0.5f, 0.0f, 0.5f);
                            editProgram.setVec3("color", c);
                            editProj = mapSc->mvpMap * glm::translate(glm::mat4(1.0f), glm::vec3(portalWorldXPixel, portalWorldYPixel, 0));
                            editProgram.setMat4("transform", &editProj);
                            editMesh.draw(editProgram);
                        }
                    }
                }
            }
            
            /* Draw doors */
            if (doorMode)
            {
                for (auto it = map->doors.begin(); it != map->doors.end(); it++)
                {
                    editProgram.setFloat("border", 0.5f);
                    
                    size_t p = it->position;
                    int doorX = p % map->width;
                    int doorY = p / map->width;
                    doorX += map->worldPos.x;
                    doorY += map->worldPos.y;
                    
                    glm::vec3 c = curDoor >= 0 && map->doors[curDoor].position == p ? glm::vec3(0.0f, 0.7f, 0.7f) : glm::vec3(0.0f, 0.2f, 0.2f);
                    editProgram.setVec3("color", c);
                    editProj = mapSc->mvpMap * glm::translate(glm::mat4(1.0f), glm::vec3(doorX * tileSize, doorY * tileSize, 0));
                    editProgram.setMat4("transform", &editProj);
                    editMesh.draw(editProgram);
                    
                    for (auto req = it->requirements.begin(); req != it->requirements.end(); req++)
                    {
                        size_t pReq = req->first;
                        int reqX = pReq % map->width;
                        int reqY = pReq / map->width;
                        reqX += map->worldPos.x;
                        reqY += map->worldPos.y;
                        
                        c = curDoor >= 0 && map->doors[curDoor].position == p ? glm::vec3(0.7f, 0.0f, 0.7f) : glm::vec3(0.2f, 0.0f, 0.2f);
                        editProgram.setFloat("border", 0.1f);
                        editProgram.setVec3("color", c);
                        editProj = mapSc->mvpMap * glm::translate(glm::mat4(1.0f), glm::vec3(reqX * tileSize, reqY * tileSize, 0));
                        editProgram.setMat4("transform", &editProj);
                        editMesh.draw(editProgram);
                    }
                }
                
            }
            
            /* Draw event triggers */
            if (eventMode)
            {
                for (size_t x = 0; x < map->width * map->height; x++)
                {
                    if (map->eventData + x && map->eventData[x].id > 0)
                    {
                        int cx = x % map->width;
                        int cy = x / map->width;
                        cx += map->worldPos.x;
                        cy += map->worldPos.y;
                        
                        editProgram.setFloat("border", 0.5f);
                        editProgram.setVec3("color", eventInd == x ? glm::vec3(1.0f, 0.0f, 1.0f) : glm::vec3(0.1f, 0.0f, 0.1f));
                        editProj = mapSc->mvpMap * glm::translate(glm::mat4(1.0f), glm::vec3(cx * tileSize, cy * tileSize, 0));
                        editProgram.setMat4("transform", &editProj);
                        editMesh.draw(editProgram);
                    }
                }
            }
            
            /* Draw claimed tiles */
            for (size_t x = 0; x < map->width * map->height; x++)
            {
                if (map->claimedData[x])
                {
                    int cx = x % map->width;
                    int cy = x / map->width;
                    cx += map->worldPos.x;
                    cy += map->worldPos.y;
                    
                    editProgram.setFloat("border", 0.5f);
                    editProgram.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
                    editProj = mapSc->mvpMap * glm::translate(glm::mat4(1.0f), glm::vec3(cx * tileSize, cy * tileSize, 0));
                    editProgram.setMat4("transform", &editProj);
                    //editMesh.draw(editProgram);
                }
            }
            
            /* Draw edit cursor */
            if (!paused)
            {
                editProj = mapSc->mvpMap * glm::translate(glm::mat4(1.0f), glm::vec3(mouseWorldXPixel, mouseWorldYPixel, 0));
                editProgram.setFloat("border", 0.1f);
                editProgram.setVec3("color", glm::vec3(1, 0, 0));
                editProgram.setFloat("alpha", 0.7f);
                editProgram.setMat4("transform", &editProj);
                editMesh.draw(editProgram);
            }
        }
        
        ImGui::Render();
        if (imGui)
        {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        
#endif
        
        int mx, my;
        int guiClick = SDL_GetMouseState(&mx, &my);
        
        /*int tileX = (mousePixelX + 7 - (pixelX / 2));
        int tileY = (mousePixelY - (pixelY / 2));
        glm::vec2 playerWorldPos = curScene->type == 0 ? curMapScene->player->worldPos : glm::vec2(0);
        glm::vec2 mouse((playerWorldPos.x * 17.0f) + tileX, (playerWorldPos.y * 17.0f) + tileY);*/
        
        glm::vec2 mouse = glm::vec2(mouseX, mouseY);
        glm::vec2 mouseUV = mouse / glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
        mouse *= glm::vec2(1920.0f / SCREEN_WIDTH, 1080.0f / SCREEN_HEIGHT);
        if (guiClick == 1 && !guiClickDown)
        {
            guiClickDown = true;
            
#ifdef DEBUG_BUILD
            std::cout << "mouse did click\n";
#endif
        }
        
        /* Draw pause menu */
        if (paused || (curScene->type == 2 && static_cast<MenuScene*>(curScene)->settingsOpen))
        {
            int mCorX = fullscreen ? (displayMode.w / 2) - (SCREEN_WIDTH / 2) : 0;
            int mCorY = fullscreen ? (displayMode.h / 2) - (SCREEN_HEIGHT / 2) : 0;
            
            mx -= mCorX;
            my -= mCorY;
            mx = glm::clamp(mx, 0, SCREEN_WIDTH);
            my = glm::clamp(my, 0, SCREEN_HEIGHT);
            
            guiBgProg.use();
            meshBg.draw(guiBgProg);
            
            guiProg.use();
            natGui.begin(SCREEN_WIDTH, SCREEN_HEIGHT, curScene->type < 2 ? 6 : 5, deltaTime);
            natGui.captureMouseInput(glm::vec2(mx, my), guiClickDown && guiClick < 1, guiClickDown);
            natGui.captureKbInput({ inputHandler->isWDown, inputHandler->isSDown, inputHandler->isADown, 
                                      inputHandler->isDDown, inputHandler->isEnterDown });
            
            int kbInd = 0;
            if (curScene->type < 2 && natGui.button(glm::vec2(825, 880), glm::vec2(250, 125), glm::vec4(0.0f, 0.5f, 0.5f, 0.0f), 0, kbInd++))
            {
                paused = false;
            }
            if (curScene->type < 2 && natGui.button(glm::vec2(825, 705), glm::vec2(250, 125), glm::vec4(0.00f, 0.5f, 0.5f, 0.0f), 1, kbInd++))
            {
                running = false;
            }
            
            if (curScene->type == 2 && natGui.button(glm::vec2(860, 705), glm::vec2(160, 80), glm::vec4(0.00f, 0.32f, 0.32f, 0.0f), 6, kbInd++))
            {
                if(curScene->type == 2) static_cast<MenuScene*>(curScene)->settingsOpen = false;
            }
            
            natGui.text(glm::vec2(865, 635), glm::vec2(175, 22.5), glm::vec4(0.0, 0.09, 0.35, 0.0), 4);
            if (natGui.slider(glm::vec2(700, 600), glm::vec2(500, 25), glm::vec4(0.00f, 0.1f, 1.0f,  0.0f), glm::vec4(0.0, 0.1, 0.1, 0.0), &musVol, 2, kbInd++))
            {
                musicMan->setVolumeMusic(musVol);
            }
            
            natGui.text(glm::vec2(885, 565), glm::vec2(325, 22.5), glm::vec4(0.35, 0.09, 1.0, 0.0), 4);
            if (natGui.slider(glm::vec2(700, 530), glm::vec2(500, 25), glm::vec4(0.00f, 0.1f, 1.0f, 0.0f), glm::vec4(0.0, 0.1, 0.1, 0.0), &fxVol, 2, kbInd++))
            {
                musicMan->setVolumeFX(fxVol);
            }
            
            if (natGui.text(glm::vec2(760, 460), glm::vec2(80, 26), glm::vec4(0.0, 0.194, 0.16, 0.09), 4) ||
                natGui.button(glm::vec2(699, 450), glm::vec2(50, 50), vsync ? glm::vec4(0.054, 0.104, 0.108, 0.0) : glm::vec4(0.0, 0.104, 0.054, 0.0), 5, kbInd++))
            {
                vsync = !vsync;
                swapSuc = SDL_GL_SetSwapInterval(vsync);
                
                if (swapSuc < 0) std::cout << SDL_GetError() << std::endl;
            }
            
            if (natGui.text(glm::vec2(760, 390), glm::vec2(125, 24), glm::vec4(0.178, 0.18, 0.43, 0.09), 4) ||
                natGui.button(glm::vec2(699, 380), glm::vec2(50, 50), fullscreen ? glm::vec4(0.054, 0.104, 0.108, 0.0) : glm::vec4(0.0, 0.104, 0.054, 0.0), 5, kbInd++))
            {
                fullscreen = !fullscreen;
                curScene->isFullScreen = fullscreen;
                
                if (fullscreen)
                {
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    mouseCorrectionX = (displayMode.w / 2) - (SCREEN_WIDTH / 2);
                    mouseCorrectionY = (displayMode.h / 2) - (SCREEN_HEIGHT / 2);
                    
                    SCREEN_WIDTH = (1.77777777778) * displayMode.h;
                    SCREEN_HEIGHT = displayMode.h;
                    
                    std::cout << "w is " << SCREEN_WIDTH << " h is " << SCREEN_HEIGHT << "\n";
                    
                    windowSize[0] = SCREEN_WIDTH;
                    windowSize[1] = SCREEN_HEIGHT;
                    
                    curScene->SCREEN_WIDTH = SCREEN_WIDTH;
                    curScene->SCREEN_HEIGHT = SCREEN_HEIGHT;
                    
                    SDL_SetWindowSize(window, displayMode.w, displayMode.h);
                    mouseCorrectionX = (displayMode.w / 2) - (SCREEN_WIDTH / 2);
                    mouseCorrectionY = (displayMode.h / 2) - (SCREEN_HEIGHT / 2);
                    
                    if(mapScene)
                    {
                        //change size
                        mapScene->isFullScreen = fullscreen;
                        mapScene->SCREEN_WIDTH = SCREEN_WIDTH;
                        mapScene->SCREEN_HEIGHT = SCREEN_HEIGHT;
                        
                        //recalculate images
                        glm::vec2 uvs[4] = { glm::vec2(0), glm::vec2(0, 1), glm::vec2(1,1), glm::vec2(1,0) };
                        int wDeath = mapScene->wDeath;
                        int hDeath = mapScene->hDeath;
                        mapScene->deathQuad.setQuad(glm::vec2((SCREEN_WIDTH - wDeath) / 2.0f, (SCREEN_HEIGHT - hDeath) / 2.0f), glm::vec2(wDeath, hDeath), uvs, SCREEN_WIDTH, SCREEN_HEIGHT);
                        
                        //recalculate frame buffer image
                        glBindFramebuffer(GL_FRAMEBUFFER, mapScene->frameBuffer);
                        glDeleteTextures(1, &mapScene->textureColorBuffer);
                        glGenTextures(1, &mapScene->textureColorBuffer);
                        glBindTexture(GL_TEXTURE_2D, mapScene->textureColorBuffer);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL); //maybe this should be constant?
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mapScene->textureColorBuffer, 0);
                    }
                }
                else
                {
                    SDL_SetWindowFullscreen(window, 0);
                    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
                    mouseCorrectionX = 0;
                    mouseCorrectionY = 0;
                }
                
            }
            
            if (natGui.text(glm::vec2(760, 320), glm::vec2(285, 24), glm::vec4(0.43, 0.18, 1.0, 0.09), 4) ||
                natGui.button(glm::vec2(699, 310), glm::vec2(50, 50), 
                              fastSnow ? glm::vec4(0.054, 0.104, 0.108, 0.0) : glm::vec4(0.0, 0.104, 0.054, 0.0), 5, kbInd++))
            {
                fastSnow = !fastSnow;
            }
            
            glEnable(GL_FRAMEBUFFER_SRGB);
            natGui.draw(guiProg);
        }
        
        /* End pause menu */
        
        if (guiClickDown && guiClick < 1)
        {
            guiClickDown = false;
        }
        
        SDL_GL_SwapWindow(window);    
    }
    
    curScene->end();
    delete mapScene;
    delete musicMan;
    
    InputHandler::CleanUp();
    TTF_Quit();
    
#ifdef DEBUG_BUILD
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    for (unsigned int i = 0; i < ids; i++) glDeleteTextures(1, &mapTexturesDbg[i]);
#endif
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    std::cout << "Goodbye!" << std::endl;
    
    return 0;
}
