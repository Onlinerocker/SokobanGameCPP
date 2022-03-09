/* date = July 10th 2021 5:25 pm */
#pragma once

#include "MapScene.h"
#include "DialogueBox.h"

namespace DialogueBuilder{
    
    bool compareStrUtf16(const char* utf16, const char* str, int len);
    size_t findCharUtf16(const char* utf16, char c, int len16);
    
    void build(const AssetManager* assMan, DialogueBox* diaBox, const char* fileName);
    void buildCutscene(const AssetManager* assMan, DialogueBox* diaBox, const char* fileName);
    void buildFromString(const AssetManager* assMan, DialogueBox* diaBox, std::string str);
    
    int getTextureByName(const AssetManager* assMan, const char* name);
    
};
