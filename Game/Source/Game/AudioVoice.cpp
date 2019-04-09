#include "GamePCH.h"
#include "AudioVoice.h"
#include "AudioManager.h"
#include "AudioEngine.h"

AudioVoice::AudioVoice(WAVEFORMATEX format, VoiceType type)
{
	m_Format = format;
	m_Type = type;
	m_IsPlaying = false;
	m_LastAudio = nullptr;
	m_Volume = 0.05f;

	AudioManager::GetEngine()->CreateAudioVoice(this);

	m_Source->SetVolume(m_Volume);
}

AudioVoice::~AudioVoice()
{
	AudioManager::GetEngine()->DestroyAudioVoice(this);
}

void AudioVoice::SetVolume(float aVolume)
{
	if (m_Source != nullptr)
	{
		m_Volume = aVolume;
		m_Source->SetVolume(m_Volume);
	}
}

float AudioVoice::GetVolume()
{
	if (m_Source != nullptr)
	{
		float volume = 0.0f;
		m_Source->GetVolume(&volume);
		return volume;
	}

	return 0.0f;
}

void AudioVoice::PlayAudio(Audio* audio)
{
	StopAudio();

	//We can only have one
	if (m_Source != nullptr)
	{
		XAUDIO2_BUFFER* Buffer = audio->GetBuffer();

		//Submit the source buffer and start playing the sound
		m_Source->SubmitSourceBuffer(Buffer);
		m_Source->Start();

		//Query the state of the buffer to calculate the sample offset, 
		//this is needed for accurate playback information
		XAUDIO2_VOICE_STATE state;
		m_Source->GetState(&state);
		audio->SetSampleOffset(state.SamplesPlayed);

		//Reset the buffer
		Buffer->PlayBegin = 0;

		//Set the is playing flag to true
		m_IsPlaying = true;
		m_LastAudio = audio;
	}
}

void AudioVoice::StopAudio()
{
	//Stop playing the sound and flush the source buffer
	if (m_Source != nullptr)
	{
		m_Source->Stop();
		m_Source->FlushSourceBuffers();

		//Set the is playing flag to false
		m_IsPlaying = false;
		m_LastAudio = nullptr;
	}
}

void AudioVoice::ImGuiGenerateVolumeSlider()
{
	ImGui::SliderFloat("Volume", &m_Volume, 0.0f, 1.0f);
	SetVolume(m_Volume);
}
