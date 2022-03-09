#include "DialogueBuilder.h"
#include <iostream>
#include <fstream>
#include <string>

bool DialogueBuilder::compareStrUtf16(const char* utf16, const char* str, int len)
{
    int x = 0;
    int y = 0;
    const Uint16* buf = reinterpret_cast<const Uint16*>(utf16);
    Uint16 g = buf[0];
    g = ((g >> 8) | (g << 8));
    
    char cur16 = (char)g;
    char curStr = str[0];
    
    while(y < len)
    {
        g = buf[x];
        g = ((g >> 8) | (g << 8));
        cur16 = (char)g;
        curStr = str[y];
        
        if(cur16 != curStr)
        {
            return false;
        }
        
        x++;
        y++;
    }
    
    return y == len;
}

size_t DialogueBuilder::findCharUtf16(const char* utf16, char c, int len16)
{
    const Uint16* buf = reinterpret_cast<const Uint16*>(utf16);
    size_t ans = len16 + 10;
    size_t i;
    for(i=0; i<len16/2; i++)
    {
        Uint16 g = buf[i];
        g = ((g >> 8) | (g << 8));
        
        //std::cout << "g is " << char(g) << "\n";
        
        if((char)g == c)
        {
            ans = i;
            break;
        }
    }
    
    return ans;
}

void DialogueBuilder::build(const AssetManager* assMan, DialogueBox* diaBox, const char* fileName)
{
    std::ifstream istream(fileName, std::ios::in);
    int line_i = 0;
    
    if(istream.is_open()){
        for(std::string line; std::getline(istream, line); )
        {
            /*diaBox->addDialogue(line, "", 
                                getTextureByName(assMan, line.c_str()), 
                                false);*/
            
            /*const Uint16* buf = reinterpret_cast<const Uint16*>(line.c_str());
            int i;
            for(i=0; i<line.length()/2; i++)
            {
                Uint16 g = buf[i];
                g = ((g >> 8) | (g << 8));
                
                if((char)g == ':')
                {
                    break;
                }
            }
            
            */
            size_t i = findCharUtf16(line.c_str(), ':', line.length());
            if(i >= line.length()/2)
            {
                continue;
            }
            
            std::string dia = line.substr((i*2)+4);
            std::string name = line.substr(line_i == 0 ? 2 : 0, line_i == 0 ? (i-1)*2 : i*2);
            bool isItalic = compareStrUtf16(name.c_str(), "NARRATION", 9);
            
            diaBox->addDialogue(dia, isItalic ? "" : name, 
                                getTextureByName(assMan, name.c_str()), 
                                isItalic);
            
            /*bool isItalic = strcmp(name.c_str(), "NARRATION") == 0;
            if(isItalic) name = "";
            diaBox->addDialogue(line.substr(p+2).c_str(), name.c_str(), 
                                getTextureByName(assMan, name.c_str()), 
                                isItalic);*/
            
            line_i++;
        }
    }
}

void DialogueBuilder::buildCutscene(const AssetManager* assMan, DialogueBox* diaBox, const char* fileName)
{
    std::ifstream istream(fileName, std::ios::in);
    std::string bgName = "h1_1";
    int line_i = 0;
    
    if(istream.is_open()){
        for(std::string line; std::getline(istream, line); )
        {
            size_t pBg = findCharUtf16(line.c_str(), '\\', line.length());//line.find("\\");
            size_t p = findCharUtf16(line.c_str(), ':', line.length());//line.find(":");
            
            if(line.length() <= 0 || p >= line.length()/2)
            {
                continue;
            }
            
            if(pBg < line.length()/2)
            {
                bgName = line.substr(line_i == 0 ? 2 : 0, pBg*2 - (line_i == 0 ? 2 : 0));
            }else
            {
                pBg = -1;
            }
            
            std::string name = line.substr((pBg+1)*2, (p-pBg)*2 - 2);
            
            //std::cout << "name " << name << "\n";
            //std::cout << "bg " << bgName << "\n";
            
            /*bool isItalic = strcmp(name.c_str(), "NARRATION") == 0;
            if(isItalic) name = "";
            diaBox->addDialogue(line.substr(p+2).c_str(), name.c_str(), 
                                -getTextureByName(assMan, bgName.c_str()), 
                                isItalic);*/
            
            bool isItalic = compareStrUtf16(name.c_str(), "NARRATION", 9);
            diaBox->addDialogue(line.substr((p+2)*2), isItalic ? "" : name, 
                                -getTextureByName(assMan, bgName.c_str()), 
                                isItalic);
            
            line_i++;
        }
    }
}

int DialogueBuilder::getTextureByName(const AssetManager* assMan, const char* name)
{
    if(compareStrUtf16(name, "Emelia", 6))
    {
        return assMan->textures[ASSET_EMELIA].id;
    }
    
    if(compareStrUtf16(name, "Ariz", 4))
    {
        return assMan->textures[ASSET_ARIZ].id;
    }
    
    return -1;
}

