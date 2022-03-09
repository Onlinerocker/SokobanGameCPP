#pragma once

#include "GL/glew.h"

#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "FontRenderer.h"

#include <vector>
#include <string>

struct DialogueLine
{
    std::string text;
    GLint charImg;
    bool isItalic;
    char imgSlot;
};

class DialogueBox
{
    private:
    
	ShaderProgram programBg;
	//ShaderProgram programChar;
	//ShaderProgram programFont;
    
	FontRenderer nameRend;
	FontRenderer nameShadowRend;
	FontRenderer dialogueRend;
    FontRenderer dialogueRendItalic;
    
	ScreenQuad textBg;
	ScreenQuad imgs[2];
    
    public:
    char curImgSlot;
	float fadeTime;
	unsigned int dialogueIndex;
	std::vector<std::string> dialogueNames;
    std::vector<DialogueLine> dialoguePairs; //each entry is a string and an image for the line of dialogue
	std::unordered_map<std::string, char> nameImgSlots;
    bool enabled;
    
	DialogueBox(TrueTypeFont* font, TrueTypeFont* fontItalic, float screenWidth, float screenHeight);
    
	void draw(ShaderProgram& programBg, ShaderProgram& programImg, ShaderProgram& programFont, float deltaTime);
	
	void setDialogue(std::string dialogue, bool isItalic);
	void setName(std::string text);
	void setImage(GLuint img, size_t slot);
	void setNoImage(size_t slot);
	void startDialogue();
	void nextDialogue();
	void addDialogue(std::string, std::string name, GLint image, bool isItalic);
};

