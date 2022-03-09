#include "QuadAnimationController.h"

QuadAnimationController::QuadAnimationController()
{
	//setState("w_up");
}

void QuadAnimationController::addState(const char* name, float frames, float duration, float uvOffset, bool xFlipped)
{
	QuadAnimationState s;
	s.duration = duration;
	s.frames = frames;
	s.uvOffset = uvOffset;
	s.xFlipped = xFlipped;

	states[name] = s;
}

void QuadAnimationController::setState(const char* name)
{
	std::map<const char*, QuadAnimationState>::iterator it = states.find(name);
	if (it != states.end())
	{
		currentState = name;
	}
}

QuadAnimationState QuadAnimationController::getCurrentState()
{
	return states[currentState];
}