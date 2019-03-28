#include "GamePCH.h"

#include "AudioManager.h"
#include "AudioEngine.h"
#include "AudioDataStructures.h"
#include "SharedAudioChannel.h"


SharedAudioChannel::SharedAudioChannel(const WAVEFORMATEX *WaveFormat)
{
	m_WaveFormat = *WaveFormat;
	m_Source = nullptr;

	if (m_Source == nullptr)
	{
		//Create the audio voice
		AudioManager::GetEngine()->CreateAudioVoice(&m_Source, &m_WaveFormat);
	}
}

SharedAudioChannel::~SharedAudioChannel()
{
	if (m_Source != nullptr)
		AudioManager::GetEngine()->DestroyAudioVoice(m_Source);
}

void SharedAudioChannel::AddAudio(Audio* audio)
{
	auto iteratorForObject = std::find(m_Audios.begin(), m_Audios.end(), audio);

	if (iteratorForObject == m_Audios.end())
	{
		audio->SetVoice(m_Source);
		m_Audios.push_back(audio);
	}
}

void SharedAudioChannel::RemoveAudio(Audio* audio)
{
	auto iteratorForObject = std::find(m_Audios.begin(), m_Audios.end(), audio);

	if (iteratorForObject != m_Audios.end())
	{
		m_Audios.erase(iteratorForObject);
	}
}

void SharedAudioChannel::PlayAudio(int index)
{
	assert(index >= 0 && index < m_Audios.size());

	m_Audios[index]->Play();
}

Audio* SharedAudioChannel::GetAudioAt(int index)
{
	assert(index >= 0 && index < m_Audios.size());

	return m_Audios[index];
}

