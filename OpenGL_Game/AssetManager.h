#pragma once
#include "Shader.h"
#include "ShaderProgram.h"
#include "TrueTypeFont.h"

#include "GL/glew.h"

#include <vector>
#include <unordered_map>

#define ASSET_TILES 0
#define ASSET_EMELIA 1
#define ASSET_ARIZ 2
#define ASSET_RANDOM 3
#define ASSET_NOISE 4
#define ASSET_PLAYER_WALK 5
#define ASSET_UI 6
#define ASSET_EMELIA_WALK 7
#define ASSET_LOGO 8
#define ASSET_MAIN_MENU 9

#define SHADER_FAST_SNOW 0
#define SHADER_CAVE 1
#define SHADER_SNOW 2
#define SHADER_NO_SNOW 3
#define SHADER_SCREEN_QUAD 4
#define SHADER_FONT 5
#define SHADER_BG 6
#define SHADER_DEFAULT 7
#define SHADER_MAP 8
#define SHADER_DYNAMIC 9
#define SHADER_PARTICLE 10
#define SHADER_CS_TRANS 11
#define SHADER_SCREEN_CUT 12
#define SHADER_MAIN_MENU 13
#define SHADER_NATGUI 14
#define SHADER_SPRITE 16

#define FONT_MSJH 0
#define FONT_ARIAL_I 1
#define FONT_ARIAL_BIG 2
#define FONT_ARIAL_BIG_OUT 3

struct AssetTextureData
{
	GLuint id;
	int width = 0;
	int height = 0;
};

class AssetManager
{
    private:
	
    //Textures
    void createAndPush(char* blob, size_t* index, size_t* size, bool flip, GLint filter);
	void createAndPushUI(char* blob, size_t* index, size_t* size, bool flip, GLint filter);
	void createAndPushUIMenu(char* blob, size_t* index, size_t* size, bool flip, GLint filter);
    void createAndPushMap(char* blob, size_t* index, size_t* size, bool flip, GLint filter);
    
    //Shaders
    void createShaderProgram(const char* vertexPath, const char* fragPath);
    
    
    public:
	std::vector<AssetTextureData> textures;
    std::unordered_map<const char*, Shader> shaders;
    std::vector<ShaderProgram> shaderPrograms;
    std::vector<TrueTypeFont*> fonts;
    
	AssetManager();
	~AssetManager();
    
};

