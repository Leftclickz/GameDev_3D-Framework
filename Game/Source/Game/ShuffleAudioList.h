#pragma once
#include "AudioList.h"

class AudioVoice;

class ShuffleAudioList : public AudioList
{
public:

	ShuffleAudioList() : AudioList() {}
	ShuffleAudioList(const WAVEFORMATEX *WaveFormat) : AudioList(WaveFormat) {}
	ShuffleAudioList(AudioVoice* voice) : AudioList(voice) {}

	virtual void AddAudio(Audio* audio) override;

	virtual void PlayAudio() override;
	virtual void Shuffle();

	virtual AudioListType GetType() override { return AudioListType_Shuffle; }

	virtual void DisplayImGuiPanel() override;


};