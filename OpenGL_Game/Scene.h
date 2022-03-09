#pragma once
class Scene
{
    
    protected:
	bool ended;
    
    public:
	bool isFullScreen;
	unsigned char type;
    
	int SCREEN_WIDTH, SCREEN_HEIGHT;
    
	virtual void start() = 0;
	virtual void run(float deltaTime) = 0;
	virtual void end() = 0;
    
};

