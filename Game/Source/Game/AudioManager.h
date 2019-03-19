#pragma once

struct WaveData;
class Audio;
class AudioEngine;


//A class that provides convenience methods to load WAV audio data and store them
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