#pragma once
#include <map>

struct QuadAnimationState
{
	float frames;
	float duration;
	float uvOffset;
	bool xFlipped;
};

class QuadAnimationController
{
private:
	std::map<const char*, QuadAnimationState> states;

	const char* currentState;

public:
	QuadAnimationController();

	void addState(const char* name, float frames, float duration, float uvOffset, bool xFlipped);
	void setState(const char* name);

	QuadAnimationState getCurrentState();

};

