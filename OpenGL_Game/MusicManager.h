#pragma once

#include "soloud.h"
#include "soloud_wav.h"

#define SOUND_CURSOR 0

class MusicManager
{

private:
	SoLoud::Soloud gSoloud;
	SoLoud::Wav gWav;
	SoLoud::Wav gWavLoop;

	SoLoud::Wav sounds[128];

	SoLoud::handle h;
	SoLoud::handle hFx;

	const char* introPath;
	const char* loopPath;

	double introLen;
	double loopLen;

	bool playingIntro;
	bool playingLoop;

public:
	float volume;
	float volumeFx;

	MusicManager();
	~MusicManager();

	void playMusicLoop(const char* introPath, const char* loopPath);
	void play(int id);
	void run();
	void setVolumeMusic(float volume);
	void setVolumeFX(float volume);
	void fadeOutMusic(float duration);

};

