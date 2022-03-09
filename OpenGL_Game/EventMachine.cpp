#include "EventMachine.h"
#include "EventTest.h"
#include "EventIntro.h"
#include "EventStartH1.h"

#include <iostream>

EventMachine::EventMachine()
{
	curEventID = 0;
    
	/* Init events */
	
    events[0] = EventPtr{};
    events[0].setup = &EventIntro::setupEvent;
    events[0].run = &EventIntro::runEvent;
    
    events[1] = EventPtr{};
	events[1].setup = &EventTest::setupEvent;
	events[1].run = &EventTest::runEvent;
    
    eventReqs[1].push_back(0);
    
    events[2] = EventPtr{};
    events[2].setup = &EventStartH1::setupEvent;
    events[2].run = &EventStartH1::runEvent;
    
    eventReqs[2].push_back(0);
    eventReqs[2].push_back(1);
}

bool EventMachine::runEvent(MapScene& scene, size_t id, const float deltaTime)
{
	if (id > 0 && curEventID != id && id <= EVENT_COUNT)
	{
		std::cout << "Starting event id " << id << "\n";
		curEventID = id;
		curEvent = &events[id - 1];
		curEvent->setup(scene);
	}
	else if(curEventID > 0)
	{
		if (!curEvent->run(scene, deltaTime))
		{
			curEventID = 0;
			return false;
		}
	}
    
	return true;
}