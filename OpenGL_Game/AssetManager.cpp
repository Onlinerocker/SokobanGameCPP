#include "AssetManager.h"
#include "TextureUtil.h"

#include <iostream>

AssetManager::AssetManager()
{
	char* assets = TextureUtil::loadBlob("Images/blob.dat");
    
	size_t index = 0;
	size_t size = 0;
	
    //Textures
	createAndPushMap(assets, &index, &size, false, GL_NEAREST); //Tile atlas texture
	createAndPush(assets, &index, &size, false, GL_LINEAR); //Emelia texture
	createAndPush(assets, &index, &size, false, GL_LINEAR); //Ariz texture
	createAndPush(assets, &index, &size, false, GL_NEAREST); //Random texture
	createAndPush(assets, &index, &size, false, GL_NEAREST); //Noise texture
	createAndPush(assets, &index, &size, false, GL_NEAREST); //Player walk texture
	createAndPushUI(assets, &index, &size, false, GL_LINEAR); //UI texture
	createAndPush(assets, &index, &size, false, GL_NEAREST); //Emelia walk texture
    createAndPush(assets, &index, &size, false, GL_LINEAR); //color logo
    createAndPushUIMenu(assets, &index, &size, false, GL_LINEAR); //UI main menu
    
    //Shaders
    createShaderProgram("Shaders/screen.vert", "Shaders/screenSnowFast.frag"); //Fast snow 0
    
    shaderPrograms[0].use();
    shaderPrograms[0].getAttribute("timeY");
    shaderPrograms[0].getAttribute("timeX");
    shaderPrograms[0].getAttribute("fadeTime");
    shaderPrograms[0].getAttribute("randomText");
    shaderPrograms[0].getAttribute("screenText");
    
    
    createShaderProgram("Shaders/screen.vert", "Shaders/screenCave.frag"); //Cave 1
    
	shaderPrograms[1].use();
	shaderPrograms[1].getAttribute("time");
	shaderPrograms[1].getAttribute("timeY");
	shaderPrograms[1].getAttribute("timeX");
	shaderPrograms[1].getAttribute("fadeTime");
	shaderPrograms[1].getAttribute("randomText");
	shaderPrograms[1].getAttribute("SIZE");
	shaderPrograms[1].setFloat("SIZE", 100.0f);
    
	createShaderProgram("Shaders/screen.vert", "Shaders/screenSnow.frag"); //Fancy Snow 2
    
	shaderPrograms[2].use();
	shaderPrograms[2].getAttribute("time");
	shaderPrograms[2].getAttribute("timeY");
	shaderPrograms[2].getAttribute("timeX");
	shaderPrograms[2].getAttribute("fadeTime");
	shaderPrograms[2].getAttribute("randomText");
	shaderPrograms[2].getAttribute("screenText");
	shaderPrograms[2].getAttribute("SIZE");
	shaderPrograms[2].setFloat("SIZE", 100.0f);
    
    
	createShaderProgram("Shaders/screen.vert", "Shaders/screenNoSnow.frag"); //No snow 3
	shaderPrograms[3].use();
	shaderPrograms[3].getAttribute("randomText");
	shaderPrograms[3].getAttribute("fadeTime");
    
	createShaderProgram("Shaders/screen.vert", "Shaders/screenImage.frag"); //Scren quad 4 
    shaderPrograms[4].use();
    shaderPrograms[4].getAttribute("color");
    shaderPrograms[4].setVec3("color", glm::vec3(1));
    
	createShaderProgram("Shaders/font.vert", "Shaders/font.frag"); //Font 5
	shaderPrograms[5].use();
	shaderPrograms[5].getAttribute("color");
	shaderPrograms[5].getAttribute("time");
    
	createShaderProgram("Shaders/screen.vert", "Shaders/txtBg.frag"); //Dialogue bg 6
    
	createShaderProgram("Shaders/vert.vert", "Shaders/frag.frag"); //Default 7
	shaderPrograms[7].use();
	shaderPrograms[7].getAttribute("diffuse");
	shaderPrograms[7].getAttribute("transform");
	shaderPrograms[7].getAttribute("time");
    
	createShaderProgram("Shaders/map.vert", "Shaders/map.frag"); //Map 8
	shaderPrograms[8].use();
	shaderPrograms[8].getAttribute("diffuse");
	shaderPrograms[8].getAttribute("transform");
	shaderPrograms[8].getAttribute("time");
	shaderPrograms[8].getAttribute("timeTrue");
    
	createShaderProgram("Shaders/vertInstance.vert", "Shaders/dynamicFrag.frag"); //Dynamic 9
	shaderPrograms[9].use();
	shaderPrograms[9].getAttribute("time");
	shaderPrograms[9].getAttribute("diffuse");
	shaderPrograms[9].getAttribute("transform");
    
	createShaderProgram("Shaders/particle.vert", "Shaders/particle.frag"); //Particle 10
	shaderPrograms[10].use();
	shaderPrograms[10].getAttribute("time");
	shaderPrograms[10].getAttribute("diffuse");
	shaderPrograms[10].getAttribute("transform");
    
    createShaderProgram("Shaders/screen.vert", "Shaders/csTrans.frag"); //Cutscene Transition 11
    shaderPrograms[11].use();
    shaderPrograms[11].getAttribute("fadeTime");
    
    createShaderProgram("Shaders/screen.vert", "Shaders/screenImageCut.frag"); //Scren quad cut scene 12 
    shaderPrograms[12].use();
    shaderPrograms[12].getAttribute("time");
    
    createShaderProgram("Shaders/screen.vert", "Shaders/mainMenu.frag"); //Main Menu 13
    shaderPrograms[13].use();
    shaderPrograms[13].getAttribute("time");
    
    createShaderProgram("Shaders/ui.vert", "Shaders/natGui.frag"); //Main Menu 13
    shaderPrograms[14].use();
    shaderPrograms[14].getAttribute("diffuse");
    
    createShaderProgram("Shaders/screen.vert", "Shaders/test.frag"); //Test 15
    
    createShaderProgram("Shaders/vert.vert", "Shaders/sprite.frag"); //Sprite 16
    shaderPrograms[16].use();
    shaderPrograms[16].getAttribute("time");
    shaderPrograms[16].getAttribute("diffuse");
    shaderPrograms[16].getAttribute("color");
    shaderPrograms[16].getAttribute("transform");
    shaderPrograms[16].setVec4("color", glm::vec4(1));
    
    /* Fonts */
    SDL_Color color{255,255,255,255};
    fonts.push_back(new TrueTypeFont("Fonts/Noto_Serif/NotoSerif-Regular.ttf", color, 48, &shaderPrograms[15])); //MSJH 0
    fonts.push_back(new TrueTypeFont("Fonts/Noto_Serif/NotoSerif-Italic.ttf", color, 48, &shaderPrograms[15])); //Arial_i 1
    
    TrueTypeFont* bigArial = new TrueTypeFont("C:/Windows/Fonts/cambriai.ttf", color, 128, &shaderPrograms[15]);
    fonts.push_back(bigArial); //Arial_big 2
    
    TrueTypeFont* bigArialOut = new TrueTypeFont("C:/Windows/Fonts/cambriai.ttf", {0,0,0,255}, 128, &shaderPrograms[15]);
    TTF_SetFontOutline(bigArialOut->uniFont, 2);
    fonts.push_back(bigArialOut); //Arial_big_out 3
}

AssetManager::~AssetManager()
{
    for (auto it = shaders.begin(); it != shaders.end(); it++)
	{
		it->second.cleanUp();
	}
    
    for (int x = 0; x < shaderPrograms.size(); x++)
	{
		shaderPrograms[x].cleanUp();
	}
    
	for (auto it = textures.begin(); it != textures.end(); it++)
	{
		glDeleteTextures(1, &it->id);
	}
}

void AssetManager::createAndPush(char* blob, size_t* index, size_t* size, bool flip, GLint filter)
{
	int width = 0;
	int height = 0;
	GLuint id = TextureUtil::createTextureFromBlob(blob, index, size, &width, &height, false, filter);
    
	AssetTextureData td = {};
	td.width = width;
	td.height = height;
	td.id = id;
    
	textures.push_back(td);
}

void AssetManager::createAndPushUI(char* blob, size_t* index, size_t* size, bool flip, GLint filter)
{
	int width = 0;
	int height = 0;
	GLuint id = TextureUtil::createUITextureArrayFromBlob(blob, index, size, &width, &height, false, filter);
    
	AssetTextureData td = {};
	td.width = width;
	td.height = height;
	td.id = id;
    
	textures.push_back(td);
}

void AssetManager::createAndPushUIMenu(char* blob, size_t* index, size_t* size, bool flip, GLint filter)
{
	int width = 0;
	int height = 0;
	GLuint id = TextureUtil::createUIMenuTextureArrayFromBlob(blob, index, size, &width, &height, false, filter);
    
	AssetTextureData td = {};
	td.width = width;
	td.height = height;
	td.id = id;
    
	textures.push_back(td);
}

void AssetManager::createAndPushMap(char* blob, size_t* index, size_t* size, bool flip, GLint filter)
{
	int width = 0;
	int height = 0;
	GLuint id = TextureUtil::createTilesTextureArrayFromBlob(blob, index, size, &width, &height, false, filter);
    
	AssetTextureData td = {};
	td.width = width;
	td.height = height;
	td.id = id;
    
	textures.push_back(td);
}

void AssetManager::createShaderProgram(const char* vertexPath, const char* fragPath)
{
    Shader* vert;
	auto vertIt = shaders.find(vertexPath);
	if (vertIt == shaders.end())
	{
		Shader vertexShader(vertexPath, GL_VERTEX_SHADER);
		shaders[vertexPath] = vertexShader;
        vert = &shaders[vertexPath];
    }
	else vert = &vertIt->second;
    
    Shader* frag;
	auto fragIt = shaders.find(fragPath);
	if (fragIt == shaders.end())
	{
		Shader fragShader(fragPath, GL_FRAGMENT_SHADER);
		shaders[fragPath] = fragShader;
        frag = &shaders[fragPath];
    }
	else frag = &fragIt->second;
    
	ShaderProgram program;
	program.attachShader(*vert);
	program.attachShader(*frag);
	program.linkProgram();
    
    shaderPrograms.push_back(program);
	
}