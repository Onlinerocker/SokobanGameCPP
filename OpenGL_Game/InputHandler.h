#pragma once
#include <SDL.h>

#include "WorldEnums.h"

class InputHandler
{
    
    private:
	static InputHandler* instance;
	uint8_t dirsSize;
    
	InputHandler();
    
	void pushDir(WorldEnums::WRL_DIR newDir);
	void popDir(WorldEnums::WRL_DIR dir);
    
    public:
	bool isWDown, isADown, isSDown, isDDown, isShiftDown,
    isRDown, isEnterDown;
    
	WorldEnums::WRL_DIR lastDirs[4]; //used for player movement input
    
	static InputHandler* GetInputHandler();
	static void CleanUp();
    
	void onKeyDown(SDL_KeyboardEvent key);
	void onKeyUp(SDL_KeyboardEvent key);
    
};

