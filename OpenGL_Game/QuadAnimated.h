#pragma once

#include "Quad.h"
#include "ShaderProgram.h"
#include "QuadAnimationController.h"

class QuadAnimated
{
    private:
	QuadAnimationController* animCon;
	Quad quad;
	float duration;
	unsigned int frames;
    
    public:
	ShaderProgram program;
    
	QuadAnimated(GLuint texture, float duration, unsigned int frames, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize);
    ~QuadAnimated();
    
	void draw(float time);
	
	QuadAnimationController* getAnimationController();
};

