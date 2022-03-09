#pragma once

#include "Scene.h"
#include "MusicManager.h"
#include "MapScene.h"

#include "SDL.h"
#include "GL/glew.h"

#include <unordered_map>

struct SceneData
{
	unsigned char type; //0 = MapScene, 1 = CutScene
	Scene* scene;
};

class SceneManager
{

private:
	TileManager* tileManager;
	GLuint mapTexture;
	MusicManager* musicManager;
	AssetManager* assMan;
	SDL_DisplayMode displayMode;
	int width, height;
	float pixelWidth, pixelHeight;

	std::unordered_map<const char*, SceneData> scenes;

public:
	SceneManager(TileManager* tileManager, GLuint mapTexture, MusicManager* musicManager, AssetManager* assMan, SDL_DisplayMode displayMode, int width, int height, float pixelWidth, float pixelHeight);
	~SceneManager(); //clean up all scenes

	bool loadSceneFromFile(const char* filePath, const char* sceneName);

	void saveSceneToFile(const char* filePath, unsigned char type, Scene* scene);

	SceneData getScene(const char* sceneName);

};

