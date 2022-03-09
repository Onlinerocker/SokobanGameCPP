#pragma once

struct GameState
{
    float playerX; //Player world X
    float playerY; //Player world Y
    int lastEvent; //Last event executed
    char mapName[128]; //Name of the currently loaded map file
};