#include "DialogueBox.h"
#include <iostream>

DialogueBox::DialogueBox(TrueTypeFont* font, TrueTypeFont* fontItalic, float screenWidth, float screenHeight)
: nameRend("???", font, 0.03 * screenWidth, 0.3 * screenHeight, screenWidth, 50, screenWidth, screenHeight),
nameShadowRend("???", font, 0.03 * screenWidth + 3, 0.3 * screenHeight, screenWidth, 50, screenWidth, screenHeight),
dialogueRend("", font, 0.03 * screenWidth, 0.3 * screenHeight - font->fontHeight, screenWidth * 0.9f, 50, screenWidth, screenHeight),
dialogueRendItalic("", fontItalic, 0.03  * screenWidth, 0.3 * screenHeight - font->fontHeight, screenWidth * 0.9f, 50, screenWidth, screenHeight), 
textBg(glm::vec2((screenWidth/2.0f) - (0.95 * (screenWidth / 2.0f)), 0.05f * screenHeight), glm::vec2(0.95f * screenWidth, 0.25f * screenHeight), screenWidth, screenHeight)
{
	fadeTime = 0.0f;
	dialogueIndex = -1;
	enabled = false;
    curImgSlot = 0;
    
    glm::vec2 imgSize = glm::vec2(screenHeight * (1550.0f / 1895.0f), screenHeight);
    imgs[0] = ScreenQuad(glm::vec2(0, 0), imgSize, screenWidth, screenHeight);
    imgs[1] = ScreenQuad(glm::vec2(screenWidth - imgSize.x, 0), imgSize, screenWidth, screenHeight);
    //if (image > 0) img.textures = { image };
}

//need to be able to update font renderer text
//need to be able to update quad images
//need a better way of passing around shaders

void DialogueBox::draw(ShaderProgram& programBg, ShaderProgram& programImg, ShaderProgram& programFont, float deltaTime)
{
	if (!enabled) return;
    DialogueLine line = dialoguePairs[dialogueIndex];
    
    for(int i = 0; i < 2; i++)
    {
        if (imgs[i].textures.size() > 0)
        {
            programImg.use();
            if(line.imgSlot == i)
            {
                programImg.setVec3("color", glm::vec3(1));
            }
            else
            {
                programImg.setVec3("color", glm::vec3(0.2));
            }
            imgs[i].draw(programImg);
        }
    }
    
	fadeTime += 20.0f * deltaTime;
    
	programBg.use();
	textBg.draw(programBg);
    
	programFont.use();
	programFont.setFloat("time", 1000.0f);
	programFont.setVec3("color", glm::vec3(0, 0, 0));
	nameShadowRend.draw(programFont);
    programFont.setVec3("color", glm::vec3(.114f, .965f, .114f));
    nameRend.draw(programFont);
    
	programFont.setFloat("time", fadeTime);
	programFont.setVec3("color", glm::vec3(1, 1, 1));
	dialogueRend.draw(programFont);
	dialogueRendItalic.draw(programFont);
    programFont.setFloat("time", 1000.0f);
}

void DialogueBox::setDialogue(std::string dialogue, bool isItalic)
{
	if(!isItalic)
    {
        //dialogueRend.setText(text);
        dialogueRendItalic.setText("");
        dialogueRend.setTextUtf16(reinterpret_cast<const Uint16*>(dialogue.c_str()), dialogue.length()/2);
    }
    else
    {
        //dialogueRendItalic.setText(text);
        dialogueRend.setText("");
        dialogueRendItalic.setTextUtf16(reinterpret_cast<const Uint16*>(dialogue.c_str()), dialogue.length()/2);
    }
}

void DialogueBox::setName(std::string name)
{
    nameRend.setTextUtf16(reinterpret_cast<const Uint16*>(name.c_str()), name.length()/2);
    nameShadowRend.setTextUtf16(reinterpret_cast<const Uint16*>(name.c_str()), name.length()/2);
	//nameRend.setText(text);
	//nameShadowRend.setText(text);
}

void DialogueBox::setImage(GLuint image, size_t slot)
{
	imgs[slot].textures = { image };
}

void DialogueBox::setNoImage(size_t slot)
{
	imgs[slot].textures = {};
}

void DialogueBox::startDialogue()
{
	if (dialoguePairs.size() < 1) return;
    
	dialogueIndex = 0.0f;
    DialogueLine line = dialoguePairs[0];
    
	//GLint image = line.charImg;
	//if (image >= 0) setImage((GLuint)image);
	//else setNoImage();
    
	setDialogue(line.text, line.isItalic);
	//dialogueRend.setTextUtf16(reinterpret_cast<const Uint16*>(line.text.c_str()), line.text.length()/2);
    setName(dialogueNames[0]);
}

void DialogueBox::nextDialogue()
{
	if (dialogueIndex < dialoguePairs.size() && enabled)
	{
        DialogueLine line = dialoguePairs[dialogueIndex];
        
		if (fadeTime*2.0f < line.text.length())
		{
			fadeTime = line.text.length();
			return;
		}
        
		unsigned int nextIndex = dialogueIndex + 1;
		if (nextIndex >= dialoguePairs.size())
		{
			fadeTime = 0.0f;
            enabled = false;
			return;
		}
        
		fadeTime = 0.0f;
		dialogueIndex++;
        
        line = dialoguePairs[dialogueIndex];
        
		//GLint image = line.charImg;
		//if (image >= 0 && line.imgSlot >= 0) setImage((GLuint)image);
		//else setNoImage();
        
		setDialogue(line.text, line.isItalic);
	}
    
    if (dialogueIndex < dialogueNames.size())
	{
		setName(dialogueNames[dialogueIndex]);
	}
}

void DialogueBox::addDialogue(std::string dialogue, std::string name, GLint image, bool isItalic)
{
    char imgSlot = -1;
    
    if(image >= 0 && strcmp(name.c_str(), "MC") != 0)
    {
        auto nameSlotsIt = nameImgSlots.find(name);
        if (nameSlotsIt == nameImgSlots.end())
        {
            //set to current image slot
            setImage((GLuint)image, curImgSlot);
            nameImgSlots[name] = curImgSlot;
            imgSlot = curImgSlot;
            curImgSlot++;
        }
        else if (nameSlotsIt != nameImgSlots.end())
        {
            imgSlot = nameSlotsIt->second;
        }
    }
    
    DialogueLine line{dialogue, image, isItalic, imgSlot};
	dialoguePairs.push_back(line);
	dialogueNames.push_back(name);
}