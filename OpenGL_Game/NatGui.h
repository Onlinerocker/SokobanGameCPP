#pragma once

#include "Mesh.h"
#include "ScreenQuad.h"
#include "MusicManager.h"

#include <vector>

//get up/down/left/right/enter input
//adjust hover ind accordingly
//if enter && hovering[indHover] =>>> didClick = true

//if kb input changes => kb mode
//if mouse pos changes => mouse mode

struct NatGuiKbInput
{
    bool up, down, left, right;
    bool select;
};

class NatGui
{
    private:
	Mesh mesh;
	
	glm::vec2 mousePos;
    
	bool didClick; //did mouse click this frame
	bool mouseDown;
	bool hovering[128];
    bool isMouseMode;
    bool turningKbMode;
    
    NatGuiKbInput prevKbIn;
    
	ShaderProgram program;
    
	int screenWidth, screenHeight;
	int scaleWidth, scaleHeight;
	int indOffset;
	int indHover;
    int indKbHover;
    int maxKbInd;
    
    float deltaTime;
    
	GLuint textureAtlas;
    
	std::vector<float> sliderPos;
	int slidersCt;
    
	MusicManager& musMan;
    
    void doPlaySound(bool isHovering, bool playSound);
	bool checkHoverPlaySound(glm::vec2 pos, glm::vec2 size, bool playSound);
    bool checkHoverPlaySoundKb(int ind, bool playSound);
	void createRect(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, float glowing, float layer);
	void convertScreenPosToUV(glm::vec2& pos);
	bool isMouseOver(glm::vec2 pos, glm::vec2 size);
    
    public:
	NatGui(GLuint textureAtlas, int screenWidth, int screenHeight, int scaleWidth, int scaleHeight, MusicManager& musMan);
    
	void begin(int screenWidth, int screenHeight, int maxKbInd, float deltaTime);
    
	/* Capture mouse input*/
	void captureMouseInput(glm::vec2 mousePos, bool didClick, bool mouseDown);
    
    /* Capture Keyboard input */
    void captureKbInput(NatGuiKbInput input);
    
	/* Create button. Return true if clicked */
	bool button(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, float layer, int kbInd);
    
	bool slider(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, glm::vec4 uvKnob, float* value, float layer, int kbInd);
    
	bool text(glm::vec2 pos, glm::vec2 size, glm::vec4 uv, float layer);
    
	/* Draw elements + background */
	void draw(ShaderProgram& program);
    
};


