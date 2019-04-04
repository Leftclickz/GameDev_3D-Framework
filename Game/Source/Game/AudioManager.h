#pragma once

//PCH audio includes
#include "AudioEngine.h"
#include "AudioDataStructures.h"
#include "AudioList.h"
#include "ShuffleAudioList.h"
#include "WeightedRandomAudioList.h"


//A class that handles all Audio related objects as well as loading WAV data files.
//Include this file to also have access to all audio-related objects.
class AudioManager
{
public:
	//create the audio manager
	static void Initialize();

	//delete the audio manager
	static void Release();

	//load a wave file.
	static bool LoadFromPath(const char* name, const char* path);

	//create an audio file with a wave file.
	static Audio* CreateAudio(const char* audioname, const char* filepath);

	static Audio* GetAudio(const std::string AudioName);
	static WaveData* GetWaveData(const std::string AudioName);
	static AudioEngine* GetEngine() { return Engine; }

private:

	static std::map<std::string, Audio*>* LoadedAudioMap;
	static std::map<std::string, WaveData*>* LoadedWaveDataMap;

	static AudioEngine* Engine;
};