#include "MusicManager.h"

#include <iostream>

MusicManager::MusicManager()
{
	this->introPath = "";
	this->loopPath = "";
	this->introLen = 0;
	this->loopLen = 0;
	this->playingIntro = false;
	this->playingLoop = false;
	this->volume = 0.04f;
	this->h = NULL;
	this->hFx = NULL;
    
	sounds[0].load("Sounds/Cursor.wav");
    
	gSoloud.init();
}

MusicManager::~MusicManager()
{
	//gSoloud.stopAll();
	gSoloud.deinit();
}

void MusicManager::playMusicLoop(const char* introPath, const char* loopPath)
{
	this->introPath = introPath;
	this->loopPath = loopPath;
    
	gWav.load(introPath);
	gWavLoop.load(loopPath);
    
	h = gSoloud.playBackground(gWav);
	gSoloud.setVolume(h, volume);
    
	introLen = gWav.getLength();
}

void MusicManager::play(int id)
{
	/*if (hFx)
	{
gSoloud.stop(hFx);
	}*/
    hFx = gSoloud.play(sounds[id], volumeFx);
}

void MusicManager::run()
{
	if (gSoloud.getStreamTime(h) > 0 && !playingIntro) playingIntro = true;
	if (gSoloud.getStreamTime(h) == 0 && playingIntro)
	{
		gWavLoop.setLooping(true);
		h = gSoloud.playBackground(gWavLoop, volume);
		playingIntro = false;
		playingLoop = true;
	}
}

void MusicManager::setVolumeMusic(float volume)
{
	this->volume = volume;
	gSoloud.setVolume(h, volume);
}

void MusicManager::setVolumeFX(float volume)
{
	this->volumeFx = volume;
	gSoloud.setVolume(hFx, volume);
}

void MusicManager::fadeOutMusic(float duration)
{
	gSoloud.fadeVolume(h, 0.0f, duration);
}