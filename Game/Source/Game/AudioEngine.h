#pragma once
#include "AudioDataStructures.h"

struct IXAudio2;
struct IXAudio2MasteringVoice;


class AudioEngine
{
	public:
		AudioEngine();
		~AudioEngine();

		//
		void CreateAudioVoice(IXAudio2SourceVoice** source, WAVEFORMATEX* waveFormat);

		//
		void DestroyAudioVoice(IXAudio2SourceVoice* source);

		//Set the volume for all the audio files being played, range 0.0f to 1.0f
		void SetVolume(float volume);

		//Returns the volume for all the audio files being played
		float GetVolume();

	private:
		IXAudio2* m_Engine;
		IXAudio2MasteringVoice* m_MasteringVoice;
		AudioCallback m_Callback;
};
