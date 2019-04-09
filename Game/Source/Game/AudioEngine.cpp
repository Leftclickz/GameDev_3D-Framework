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
			voice->SetVolume(0.3f);
		}
	}

	void AudioEngine::StopChannelsOfType(enum VoiceType type = VoiceType_Default)
	{
		//if its default then stop everything
		if (type == VoiceType_Default)
		{
			for (unsigned int i = 0; i < m_Channels.size(); i++)
				m_Channels[i]->StopAudio();

			return;
		}

		for (unsigned int i = 0; i < m_Channels.size(); i++)
		{
			if (m_Channels[i]->GetVoiceType() == type)
			{
				m_Channels[i]->StopAudio();
			}
		}
	}

	void AudioEngine::ImGuiDisplayChannels()
	{
		ImGui::Begin("Sound");
		ImGui::PushID(this);

		if (ImGui::TreeNode("Sound Channels"))
		{
			if (ImGui::Button("Stop All Channels"))
				StopChannelsOfType();

			if (ImGui::CollapsingHeader("Public Channels"))
			{
				if (ImGui::Button("Stop Channels"))
					StopChannelsOfType(VoiceType_Public);
				GenerateImGuiChannelTreeNodes(VoiceType_Public);
			}

			if (ImGui::CollapsingHeader("Shared Audio Channels"))
			{
				if (ImGui::Button("Stop Channels"))
					StopChannelsOfType(VoiceType_Shared);
				GenerateImGuiChannelTreeNodes(VoiceType_Shared);
			}

			if (ImGui::CollapsingHeader("Dedicated Audio Channels"))
			{
				if (ImGui::Button("Stop Channels"))
					StopChannelsOfType(VoiceType_Dedicated);
				GenerateImGuiChannelTreeNodes(VoiceType_Dedicated);
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
		ImGui::End();
	}

	void AudioEngine::GenerateImGuiChannelTreeNodes(enum VoiceType type)
	{
		int index = 1;
		std::string voice_type_string;

		//enum to string
		switch (type)
		{
		case VoiceType_Default:
			voice_type_string = "DEFAULT - ";
			break;
		case VoiceType_Dedicated:
			voice_type_string = "Dedicated - ";
			break;
		case VoiceType_Shared:
			voice_type_string = "Shared - ";
			break;
		case VoiceType_Public:
			voice_type_string = "Public - ";
			break;
		default:
			break;

		}

		//print all nodes of the selected channel type
		for (unsigned int i = 0; i < m_Channels.size(); i++)
		{
			if (m_Channels[i]->GetVoiceType() == type)
			{
				//generate a display name
				std::string display = voice_type_string + std::to_string(index);
				index++;

				if (ImGui::TreeNode(display.c_str()))
				{
					//display last played audio if there is one
					{
						std::string current_audio_playing_string;
						if (m_Channels[i]->GetCurrentAudioPlaying() != nullptr)
							current_audio_playing_string = m_Channels[i]->GetCurrentAudioPlaying()->GetName();
						else
							current_audio_playing_string = "NONE";

						std::string audio_display = "Current Audio Playing - " + current_audio_playing_string;

						ImGui::Text(audio_display.c_str());
					}

					//generate a volume slider 
					m_Channels[i]->ImGuiGenerateVolumeSlider();

					//stop active audio
					if (ImGui::Button("Stop"))
					{
						m_Channels[i]->StopAudio();
					}

					ImGui::TreePop();
				}
			}
		}
	}
