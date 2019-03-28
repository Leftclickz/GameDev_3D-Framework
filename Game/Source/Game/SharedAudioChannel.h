#pragma once

class SharedAudioChannel
{
public:

	SharedAudioChannel(const WAVEFORMATEX *WaveFormat);
	~SharedAudioChannel();

	void AddAudio(Audio* audio);

	void RemoveAudio(Audio* audio);

	void PlayAudio(int index);

	Audio* GetAudioAt(int index);

private:


	IXAudio2SourceVoice* m_Source;
	WAVEFORMATEX m_WaveFormat;

	std::vector<Audio*> m_Audios;
};

