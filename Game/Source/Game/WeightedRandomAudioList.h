#pragma once
#include "AudioList.h"

class AudioVoice;

class WeightedRandomAudioList : public AudioList
{
public:

	WeightedRandomAudioList() : AudioList() {}
	WeightedRandomAudioList(const WAVEFORMATEX *WaveFormat) : AudioList(WaveFormat) {}
	WeightedRandomAudioList(AudioVoice* voice) : AudioList(voice) {}

	//adds audio to the list and gives a default weight of 10
	virtual void AddAudio(Audio* audio) override;

	//override to assign a weight value beforehand
	virtual void AddAudio(Audio* audio, int value);

	//remove audio from the list and remove the weight
	virtual void RemoveAudio(Audio* audio) override;

	//set the weight of a specific piece of audio 
	virtual void SetAudioWeightValue(Audio* audio, int value);

	//play a random audio using weights
	virtual void PlayAudio() override;

	virtual AudioListType GetType() override { return AudioListType_WeightedRandom; }

	virtual void DisplayImGuiPanel() override;

protected:

	virtual int GetWeightCount();

	unsigned int m_MostRecentlyPlayedIndex;
	std::vector<int> m_AudioWeights;
};