#include "InputHandler.h"

#include <iostream>

InputHandler* InputHandler::instance;

InputHandler::InputHandler()
{
	isWDown = false;
	isADown = false;
	isSDown = false;
	isDDown = false;
	isShiftDown = false;
	isRDown = false;
    isEnterDown = false;
    
	instance = NULL;
    
	for (int i = 0; i < 4; i++) lastDirs[i] = WorldEnums::WRL_DIR::NIL;
}

InputHandler* InputHandler::GetInputHandler()
{
	if (!instance)
	{
		instance = new InputHandler();
	}
    
	return instance;
}

void InputHandler::pushDir(WorldEnums::WRL_DIR newDir)
{
	lastDirs[3] = lastDirs[2];
	lastDirs[2] = lastDirs[1];
	lastDirs[1] = lastDirs[0];
	lastDirs[0] = newDir;
}

void InputHandler::popDir(WorldEnums::WRL_DIR dir)
{
    
	for (uint8_t i = 0; i < 4; i++)
	{
		if (lastDirs[i] == dir)
		{
			dirsSize--;
			for (int j = i; j < 3; j++)
			{
				lastDirs[j] = lastDirs[j + 1];
			}
			lastDirs[3] = WorldEnums::WRL_DIR::NIL;
			break;
		}
	}
}

void InputHandler::CleanUp()
{
	delete instance;
}

void InputHandler::onKeyDown(SDL_KeyboardEvent key)
{
	// WASD
	if (key.keysym.scancode == SDL_SCANCODE_W)
	{
		if(!isWDown) pushDir(WorldEnums::WRL_DIR::UP);
		isWDown = true;
	}
	else if (key.keysym.scancode == SDL_SCANCODE_A)
	{
		if(!isADown) pushDir(WorldEnums::WRL_DIR::LEFT);
		isADown = true;
	}
	else if (key.keysym.scancode == SDL_SCANCODE_S)
	{
		if (!isSDown) pushDir(WorldEnums::WRL_DIR::DOWN);
		isSDown = true;
	}
	else if (key.keysym.scancode == SDL_SCANCODE_D)
	{
		if (!isDDown) pushDir(WorldEnums::WRL_DIR::RIGHT);
		isDDown = true;
	}
	
	if (key.keysym.scancode == SDL_SCANCODE_LSHIFT)
	{
		isShiftDown = true;
	}
    
	if (key.keysym.scancode == SDL_SCANCODE_R)
	{
		isRDown = true;
	}
    
    if (key.keysym.scancode == SDL_SCANCODE_RETURN || key.keysym.scancode == SDL_SCANCODE_J)
	{
		isEnterDown = true;
	}
}

void InputHandler::onKeyUp(SDL_KeyboardEvent key)
{
	// WASD
	if (key.keysym.scancode == SDL_SCANCODE_W)
	{
		isWDown = false;
		popDir(WorldEnums::WRL_DIR::UP);
	}
    
	if (key.keysym.scancode == SDL_SCANCODE_A)
	{
		isADown = false;
		popDir(WorldEnums::WRL_DIR::LEFT);
	}
    
	if (key.keysym.scancode == SDL_SCANCODE_S)
	{
		isSDown = false;
		popDir(WorldEnums::WRL_DIR::DOWN);
	}
    
	if (key.keysym.scancode == SDL_SCANCODE_D)
	{
		isDDown = false;
		popDir(WorldEnums::WRL_DIR::RIGHT);
	}
	
	if (key.keysym.scancode == SDL_SCANCODE_LSHIFT)
	{
		isShiftDown = false;
	}
    
	if (key.keysym.scancode == SDL_SCANCODE_R)
	{
		isRDown = false;
	}
    
    if (key.keysym.scancode == SDL_SCANCODE_RETURN || key.keysym.scancode == SDL_SCANCODE_J)
	{
		isEnterDown = false;
	}
}