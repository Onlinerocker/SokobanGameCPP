#pragma once
#include "Event.h"
#include "EventTest.h"

#define EVENT_COUNT 3

class MapScene;

struct EventPtr
{
	void(*setup)(MapScene&);
	bool(*run)(MapScene&, float);
};

class EventMachine
{
    
    private:
	EventPtr events[10];
	EventPtr* curEvent;
	//Event* curEvent;
    
    public:
    std::vector<int> eventReqs[10];
	unsigned int curEventID;
	EventMachine();
	bool runEvent(MapScene& scene, size_t id, const float deltaTime);
    std::vector<int> getEventReqs(size_t id);
};

