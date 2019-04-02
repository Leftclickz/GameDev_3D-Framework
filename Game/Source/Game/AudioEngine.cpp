#include "GamePCH.h"
#include "AudioEngine.h"
#include "AudioVoice.h"


WAVEFORMATEX AudioEngine::DEFAULT_WAVE_FORMAT;
const unsigned int AudioEngine::MAX_PUBLIC_AUDIO_CHANNELS = 10;

	AudioEngine::AudioEngine() :
		m_Engine(nullptr),
		m_MasteringVoice(nullptr),
		m_Callback()
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		//Create the engine
		HRESULT result = XAudio2Create(&m_Engine);
		assert(SUCCEEDED(result));

		//Create the mastering voice
		result = m_Engine->CreateMasteringVoice(&m_MasteringVoice);
		assert(SUCCEEDED(result));
	}

	AudioEngine::~AudioEngine()
	{
		//iterate through each element and destroy their voices
		while (m_Channels.size() > 0)
		{
			m_Channels[0]->StopAudio();
			delete m_Channels[0];
		}

		m_MasteringVoice->DestroyVoice();
		m_Engine->Release();
		CoUninitialize();
	}

	void AudioEngine::CreateAudioVoice(AudioVoice* m_Voice)
	{
		//fetch format and empty voice pointer
		IXAudio2SourceVoice* aSource;
		WAVEFORMATEX aWaveFormat = m_Voice->GetFormat();
		
		//create source voice
		HRESULT result = m_Engine->CreateSourceVoice(&aSource, &aWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_Callback, nullptr, nullptr);
		assert(SUCCEEDED(result));
		m_Voice->SetSource(aSource);

		//add to the vector of managed voices
		m_Channels.push_back(m_Voice);
	}

	void AudioEngine::DestroyAudioVoice(AudioVoice* m_Voice)
	{
		//find our source, destroy it
		IXAudio2SourceVoice* aSource = m_Voice->GetSource();
		if (aSource != nullptr)
		{
			aSource->DestroyVoice();
			aSource = nullptr;
		}

		//remove from vector
		auto iteratorForObject = std::find(m_Channels.begin(), m_Channels.end(), m_Voice);

		if (iteratorForObject != m_Channels.end())
		{
			m_Channels.erase(iteratorForObject);
		}
	}

	void AudioEngine::SetVolume(float aVolume)
	{
		m_MasteringVoice->SetVolume(aVolume);
	}

	float AudioEngine::GetVolume()
	{
		float volume = 0.0f;
		m_MasteringVoice->GetVolume(&volume);
		return volume;
	}

	AudioVoice* AudioEngine::GetAvailablePublicChannel()
	{
		//find an empty channel
		for (unsigned int i = 0; i < m_Channels.size(); i++)
		{
			if (m_Channels[i]->GetVoiceType() == VoiceType_Public && m_Channels[i]->IsPlaying() == false)
			{
				return m_Channels[i];
			}
		}

		//if there isnt any shut one off at random and return that.
		while (true)
		{
			int random = rand() % m_Channels.size();

			if (m_Channels[random]->GetVoiceType() == VoiceType_Public)
			{
				m_Channels[random]->StopAudio();
				return m_Channels[random];
			}
		}
		return nullptr;
	}

	void AudioEngine::SetDefaultWaveFormat(WAVEFORMATEX format)
	{
		DEFAULT_WAVE_FORMAT = format;
	}

	void AudioEngine::CreatePublicAudioChannels()
	{
		m_Channels.reserve(MAX_PUBLIC_AUDIO_CHANNELS);
		for (unsigned int i = 0; i < MAX_PUBLIC_AUDIO_CHANNELS; i++)
		{
			AudioVoice* voice = new AudioVoice(DEFAULT_WAVE_FORMAT, VoiceType_Public);
		}
	}

	void AudioEngine::StopAllPublicAudioChannels()
	{
		for (unsigned int i = 0; i < m_Channels.size(); i++)
		{
			if (m_Channels[i]->GetVoiceType() == VoiceType_Public)
			{
				m_Channels[i]->StopAudio();
			}
		}
	}
