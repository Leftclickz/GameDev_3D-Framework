#pragma once

class Audio;

enum VoiceType
{
	VoiceType_Default,		//default, this means the channel was created wrong somehow
	VoiceType_Dedicated,	//dedicated : only a single audio object will ever use this channel
	VoiceType_Shared,		//shared : An audio list object is using this channel
	VoiceType_Public,		//public : This channel is managed by the engine and plays any audios without dedicated or shared channels
};

class AudioVoice
{

public:
	AudioVoice(WAVEFORMATEX format, VoiceType type = VoiceType_Default);
	~AudioVoice();

	void SetVolume(float volume);
	float GetVolume();

	VoiceType GetVoiceType() { return m_Type; }

	void PlayAudio(Audio* audio);
	void StopAudio();

	bool IsPlaying() { return m_IsPlaying; }

	IXAudio2SourceVoice* GetSource() { return m_Source; }
	void SetSource(IXAudio2SourceVoice* Source) { m_Source = Source; }

	WAVEFORMATEX GetFormat() { return m_Format; }

	Audio* GetCurrentAudioPlaying() { return m_LastAudio; }

	void ImGuiGenerateVolumeSlider();
	float* GetVolumePointer() { return &m_Volume; }

protected:

	IXAudio2SourceVoice* m_Source;
	WAVEFORMATEX m_Format;
	VoiceType m_Type;
	bool m_IsPlaying;
	Audio* m_LastAudio;
	float m_Volume;
};