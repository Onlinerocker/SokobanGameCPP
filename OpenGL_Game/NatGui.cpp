#include "NatGui.h"

#include <iostream>

NatGui::NatGui(GLuint textureAtlas, int screenWidth, int screenHeight, int scaleWidth, int scaleHeight, MusicManager& musMan) :
musMan(musMan)
{
	didClick = false;
	mouseDown = false;
    isMouseMode = true;
	mousePos = glm::vec2(0);
    
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->scaleWidth = scaleWidth;
	this->scaleHeight = scaleHeight;
	this->textureAtlas = textureAtlas;
    
	indOffset = 0;
	slidersCt = 0;
    indKbHover = 0;
    maxKbInd = 0;
    deltaTime = 0.0f;
    
    prevKbIn = {false, false, false, false, false};
    
	Texture t;
	t.id = textureAtlas;
	t.type = diffuse;
	mesh.textures = { t };
    
	for (int x = 0; x < 128; x++) hovering[x] = false;
}

void NatGui::createRect(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, float glowing, float layer)
{
	GLuint inds[6] = { 0, 1, 2, 0, 2, 3 };
	VertexUI v{};
    
	v.vertex = pos;
	v.uv = glm::vec2(uv.x, uv.y);
	v.glowing = glowing;
	v.layer = layer;
	convertScreenPosToUV(v.vertex);
	mesh.verticesUi.push_back(v);
    
	v.vertex = pos + glm::vec2(0, size.y);
	v.uv = glm::vec2(uv.x, uv.w);
	v.glowing = glowing;
	v.layer = layer;
	convertScreenPosToUV(v.vertex);
	mesh.verticesUi.push_back(v);
    
	v.vertex = pos + size;
	v.uv = glm::vec2(uv.z, uv.w);
	v.glowing = glowing;
	v.layer = layer;
	convertScreenPosToUV(v.vertex);
	mesh.verticesUi.push_back(v);
    
	v.vertex = pos + glm::vec2(size.x, 0);
	v.uv = glm::vec2(uv.z, uv.y);
	v.glowing = glowing;
	v.layer = layer;
	convertScreenPosToUV(v.vertex);
	mesh.verticesUi.push_back(v);
    
	for (int x = 0; x < 6; x++)
	{
		mesh.indices.push_back(inds[x] + indOffset);
	}
    
	indOffset += 4;
}

void NatGui::convertScreenPosToUV(glm::vec2& pos)
{
	pos.x /= scaleWidth;
	pos.y /= scaleHeight;
    
	pos.x = -1 + (pos.x * 2.0);
	pos.y = -1 + (pos.y * 2.0);
}

bool NatGui::isMouseOver(glm::vec2 pos, glm::vec2 size)
{
	glm::vec2 mp = mousePos;
    
	mp.y = screenHeight - mp.y;
    
	mp.x *= (static_cast<float>(scaleWidth) / static_cast<float>(screenWidth));
	mp.y *= (static_cast<float>(scaleHeight) / static_cast<float>(screenHeight));
    
	return mp.x >= pos.x && mp.x < pos.x + size.x && mp.y >= pos.y && mp.y < pos.y + size.y;
}

void NatGui::doPlaySound(bool isHovering, bool playSound)
{
    if (isHovering && !hovering[indHover])
	{
		hovering[indHover] = true;
		if(playSound) musMan.play(SOUND_CURSOR);
	}
	else if (!isHovering && hovering[indHover])
	{
		hovering[indHover] = false;
	}
}

bool NatGui::checkHoverPlaySound(glm::vec2 pos, glm::vec2 size, bool playSound)
{
	bool h = isMouseOver(pos, size);
	doPlaySound(h, playSound);
	return h;
}

bool NatGui::checkHoverPlaySoundKb(int ind, bool playSound)
{
    bool h = ind == indKbHover;
	doPlaySound(h, playSound);
	return h;
}

void NatGui::begin(int screenWidth, int screenHeight, int maxKbInd, float deltaTime)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
    this->maxKbInd = maxKbInd;
    this->deltaTime = deltaTime;
    
	indOffset = 0;
    
	mesh.verticesUi.clear();
	mesh.indices.clear();
	slidersCt = 0;
	indHover = 0;
    
    didClick = false;
}

void NatGui::captureMouseInput(glm::vec2 mousePos, bool didClick, bool mouseDown)
{
    if(this->mousePos.x != mousePos.x || this->mousePos.y != mousePos.y)
    {
        isMouseMode = true;
    }
    
	this->mousePos = mousePos;
	this->didClick = didClick;
	this->mouseDown = mouseDown;
}

void NatGui::captureKbInput(NatGuiKbInput input)
{
    /* If in keyboard mode */
    if(!isMouseMode)
    {
        if(input.up && input.up != prevKbIn.up)
        {
            if(!turningKbMode && indKbHover > 0) indKbHover--;
            else
            {
                turningKbMode = false;
                input.up = false;
            }
        }
        
        if(input.down && input.down != prevKbIn.down)
        {
            if(!turningKbMode && indKbHover < maxKbInd) indKbHover++;
            else
            {
                turningKbMode = false;
                input.down = false;
            }
        }
        
        if(!input.select && prevKbIn.select)
        {
            didClick = true;
        }
    }
    else
    {
        /* See if we can enter mouse mode */
        if(input.up != prevKbIn.up)
        {
            isMouseMode = false;
            turningKbMode = true;
            //indKbHover -= 1;
        }
        
        if(input.down != prevKbIn.down)
        {
            isMouseMode = false;
            turningKbMode = true;
            //indKbHover -= 1;
        }
        
        if(input.left != prevKbIn.left)
        {
            isMouseMode = false;
            turningKbMode = true;
            //indKbHover -= 1;
        }
        
        if(input.right != prevKbIn.right)
        {
            isMouseMode = false;
            turningKbMode = true;
            //indKbHover -= 1;
        }
    }
    
    prevKbIn = input;
}

bool NatGui::button(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, float layer, int kbInd)
{
	bool h = isMouseMode ? checkHoverPlaySound(pos, size, true) : checkHoverPlaySoundKb(kbInd, true);
    if(h && isMouseMode)
    {
        indKbHover = kbInd;
    }
    
	createRect(pos, size, uv, h ? 1.0f : 0.0f, layer);
	indHover++;
	return h && didClick;
}

bool NatGui::slider(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, glm::vec4 uvKnob, float* value, float layer, int kbInd)
{
	glm::vec2 knobSize = size;
	knobSize.x /= 10.0f;
	knobSize.y *= 1.5f;
    
	if (slidersCt + 1 > sliderPos.size())
	{
		sliderPos.push_back(pos.x + (*value * size.x) + (knobSize.x / 2.0f));
	}
    
	glm::vec2 knobPos = pos;
	knobPos.x = glm::clamp(sliderPos[slidersCt] - (knobSize.x / 2.0f), pos.x, pos.x + size.x - knobSize.x);
	knobPos.y -= (size.y * 0.25f);
    
	glm::vec2 clickSize = size;
	clickSize.y = knobSize.y;
    
	bool hoveringKnob = isMouseMode ? checkHoverPlaySound(pos, size, true) : checkHoverPlaySoundKb(kbInd, true);
	createRect(pos, size, uv, 0.0f, layer);
	createRect(knobPos, knobSize, uvKnob, hoveringKnob ? 1.0f : 0.0f, layer+1);
    
	bool h = isMouseOver(pos, size);
	bool clicking = h && mouseDown;
	if (clicking && isMouseMode)
	{
		sliderPos[slidersCt] = mousePos.x * (static_cast<float>(scaleWidth) / static_cast<float>(screenWidth));
        
		*value = glm::clamp((((sliderPos[slidersCt] - (knobSize.x / 2.0f)) - pos.x) / size.x), 0.0f, 1.0f);
		*value = glm::round(static_cast<double>(*value * 100.0f));
		*value /= 100.0f;
        
        if (*value < 0.001f) *value = 0.0f;
        if (*value >= 0.995f) *value = 1.0f;
    }
    else if (!isMouseMode)
    {
        if(prevKbIn.right && kbInd == indKbHover)
        {
            *value += 0.3f * deltaTime;
            sliderPos[slidersCt] = (pos.x + knobSize.x / 2.0f) + (*value * size.x);
            clicking = true;
        }
        else if(prevKbIn.left && kbInd == indKbHover)
        {
            *value -= 0.3f * deltaTime;
            sliderPos[slidersCt] = (pos.x + knobSize.x / 2.0f) + (*value * size.x);
            clicking = true;
        }
        
        if (*value < 0.0f) *value = 0.0f;
        if (*value > 1.0f) *value = 1.0f;
    }
    
	indHover++;
	slidersCt++;
	return clicking;
}

bool NatGui::text(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, float layer)
{
	bool h = checkHoverPlaySound(pos, size, false);
	createRect(pos, size, uv, 0.0f, layer);
	indHover++;
	return h && didClick;
}

void NatGui::draw(ShaderProgram& program)
{
	mesh.setupUiMesh();
	mesh.drawArray(program);
}