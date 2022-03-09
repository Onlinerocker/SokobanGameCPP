#include "QuadAnimated.h"

#include "TextureUtil.h"
#include <iostream>

QuadAnimated::QuadAnimated(GLuint texture, float duration, unsigned int frames, glm::vec3 location, glm::vec2 size, glm::vec2 uvSize) :
quad(texture, location, size, uvSize)
{
	this->frames = frames;
	this->duration = duration;
    
	animCon = new QuadAnimationController();
    
	Shader vertexShader("Shaders/vert.vert", GL_VERTEX_SHADER);
	Shader pixelShader("Shaders/quadAnim.frag", GL_FRAGMENT_SHADER);
    
	program.attachShader(vertexShader);
	program.attachShader(pixelShader);
	program.linkProgram();
    
	GLint success = GL_FALSE;
	glGetProgramiv(program.getShaderProgram(), GL_LINK_STATUS, &success);
	if (!success) std::cout << "[OpenGL] Could not link shader program." << std::endl;
    
	program.use();
	program.getAttribute("duration");
	program.setFloat("duration", duration);
    
	program.getAttribute("frames");
	program.setFloat("frames", (float)frames);
    
	program.getAttribute("animTime");
	program.setFloat("animTime", 0.0f);
    
	program.getAttribute("uvOffset");
	program.setFloat("uvOffset", 0.0f);
    
	program.getAttribute("xFlipped");
	program.setInt("xFlipped", 0);
    
	program.getAttribute("transform");
	program.getAttribute("rotation");
}

QuadAnimated::~QuadAnimated()
{
	delete animCon;
}

//animation controller
//  set state
//  based on state, choose correct UVs in sprite sheet
//  each state will have an animation
//  each animation will have a subset of frames, duration
//  pass correct state to animation shader (this is the UV y offset)
//  the UV x offset is the frame of animation
//  pass this animation controller to the QuadAnimated
//  before draw
//    set frames for state
//    set duration for state
//    set time as usual

//make sure to set projection matrix before calling this
void QuadAnimated::draw(float time)
{
	QuadAnimationState state = animCon->getCurrentState();
    
	program.use();
	program.setFloat("duration", state.duration);
	program.setFloat("frames", state.frames);
	program.setFloat("uvOffset", state.uvOffset);
	program.setFloat("animTime", time);
	program.setInt("xFlipped", static_cast<GLint>(state.xFlipped));
    
	quad.draw(program);
}

QuadAnimationController* QuadAnimated::getAnimationController()
{
	return animCon;
}