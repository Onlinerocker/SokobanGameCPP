#include "SceneManager.h"

#include <fstream>
#include <iostream>

SceneManager::SceneManager(TileManager* tileManager, GLuint mapTexture, MusicManager* musicManager, AssetManager* assMan, SDL_DisplayMode displayMode, int width, int height, float pixelWidth, float pixelHeight)
{
	this->tileManager = tileManager;
	this->mapTexture = mapTexture;
	this->musicManager = musicManager;
	this->assMan = assMan;
	this->displayMode = displayMode;
	this->width = width;
	this->height = height;
	this->pixelWidth = pixelWidth;
	this->pixelHeight = pixelHeight;
}

SceneManager::~SceneManager()
{
	for (auto it=scenes.begin(); it != scenes.end(); it++)
	{
		delete it->second.scene;
	}
}

bool SceneManager::loadSceneFromFile(const char* filePath, const char* sceneName)
{
	std::ifstream istream(filePath, std::ios::in | std::ios::binary);

	if (istream.is_open())
	{
		//Get scene type
		char t;
		istream.read(&t, 1);
		unsigned char type = static_cast<unsigned char>(t);

		SceneData d;
		d.type = type;

		switch (type)
		{

		case 0:
		{
			//Get size of Map file name
			char* fc = new char[sizeof(size_t)];
			istream.read(fc, sizeof(size_t));
			size_t fileCount = static_cast<size_t>(*fc);

			//std::cout << "Loaded file count " << fileCount << std::endl;

			//Get map file name
			char* fileName = new char[fileCount + 1];
			istream.read(fileName, fileCount);
			fileName[fileCount] = '\0';

			//std::cout << "Loaded map " << reinterpret_cast<const char*>(fileName) << "|end" << std::endl;

			//Get player pos
			size_t posSize = sizeof(float) * 3;
			char* playerPos = new char[posSize];
			istream.read(playerPos, posSize);
			float playerPosF[3] = { static_cast<float>(playerPos[0]),
									static_cast<float>(playerPos[sizeof(float)]),
									static_cast<float>(playerPos[sizeof(float) * 2]) };

			//std::cout << "Loaded player pos " << playerPosF[0] << "," << playerPosF[1] << "," << playerPosF[2] << "," << std::endl;

			//Load map
			Map* map = new Map(tileManager, glm::vec3(0, 0, 0), 17.0f, 10, 10);
			map->loadMapFromFile(reinterpret_cast<const char*>(fileName));

			//Create scene
			MapScene* mapScene = new MapScene(map, mapTexture, musicManager, this->assMan, displayMode, width, height, pixelWidth, pixelHeight);
			mapScene->player->setPos(glm::vec3(playerPosF[0], playerPosF[1], playerPosF[2]));

			d.scene = mapScene;
			scenes[sceneName] = d;

			delete[] fileName;
			delete[] playerPos;
			break;
		}
		case 1:
			break;

		default:
			break;

		}

		istream.close();
		return true;
	}

	istream.close();
	return false;
}

void SceneManager::saveSceneToFile(const char* filePath, unsigned char type, Scene* scene)
{
	std::ofstream ostream(filePath, std::ios::out | std::ios::binary);

	if (ostream.is_open())
	{
		switch (type)
		{
		case 0:
		{
			MapScene* mapScene = static_cast<MapScene*>(scene);

			//Write type
			char t = static_cast<char>(type);
			ostream.write(&t, 1);

			//Write length of map file name
			const char* fileName = mapScene->map->getMapFileName();
			size_t mapNameLen = strlen(fileName);
			ostream.write(reinterpret_cast<char*>(&mapNameLen), sizeof(size_t));

			//Write map file name
			ostream.write(fileName, mapNameLen);

			//Write player pos
			float playerPos[3] = { mapScene->player->pos.x,
									mapScene->player->pos.y,
									mapScene->player->pos.z };
			ostream.write(reinterpret_cast<char*>(playerPos), 3 * sizeof(float));

			break;
		}
		case 1:
			break;
		
		default:
			break;
		}
	}

	ostream.close();
}

SceneData SceneManager::getScene(const char* sceneName)
{
	return scenes[sceneName];
}