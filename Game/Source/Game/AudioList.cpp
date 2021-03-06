#include "GamePCH.h"
#include "AudioList.h"
#include "AudioVoice.h"
#include "AudioDataStructures.h"

AudioList::AudioList()
{
}

AudioList::AudioList(AudioVoice* voice)
{
}

AudioList::AudioList(const WAVEFORMATEX *WaveFormat)
{
	//Create the audio voice
	m_Voice = new AudioVoice(*WaveFormat, VoiceType_Shared);
}



void AudioList::Update(float deltatime)
{
	if (isEnabled == false)
		StopAudio();
}

AudioList::~AudioList()
{

}

void AudioList::AddAudio(Audio* audio)
{
	auto iteratorForObject = std::find(m_Audios.begin(), m_Audios.end(), audio);

	if (iteratorForObject == m_Audios.end())
	{
		audio->SetVoice(m_Voice);
		m_Audios.push_back(audio);
	}
}

void AudioList::RemoveAudio(Audio* audio)
{
	auto iteratorForObject = std::find(m_Audios.begin(), m_Audios.end(), audio);

	if (iteratorForObject != m_Audios.end())
	{
		m_Audios.erase(iteratorForObject);
	}

}

void AudioList::PlayAudio(unsigned int index)
{
	assert(index >= 0 && index < m_Audios.size());

	if (isEnabled)
		m_Audios[index]->Play(IsUsingPublicChannels());
}

void AudioList::PlayAudio()
{
	if (isEnabled)
	{
		m_Audios[m_CurrentAudioToPlay]->Play(IsUsingPublicChannels());
		m_LastPlayed = m_Audios[m_CurrentAudioToPlay];

		m_CurrentAudioToPlay++;
		m_AudioPlaybackCount++;

		//clamp for rerolling
		if (m_CurrentAudioToPlay == m_Audios.size())
		{
			m_CurrentAudioToPlay = 0;

		}
	}
}

void AudioList::StopAudio()
{
	if (IsUsingPublicChannels() == false)
	{
		m_Voice->StopAudio();
	}
}

void AudioList::SetName(std::string name)
{
	m_Name = name;
	GenerateImguiDisplayString();
}

void AudioList::DisplayImGuiPanel()
{

	ImGui::Checkbox("Enabled", &isEnabled);

	if (isEnabled == true)
	{
		std::string last_display;
		if (m_LastPlayed != nullptr)
			last_display = "Last Audio Played: " + std::string(m_LastPlayed->GetName());
		else
			last_display = "Last Audio Played: NONE";

		if (IsUsingPublicChannels() == false)
		{
			if (ImGui::Button("Stop"))
				StopAudio();

			ImGui::SliderFloat("Volume", m_Voice->GetVolumePointer(), 0.0f, 1.0f);
			m_Voice->SetVolume(*m_Voice->GetVolumePointer());
		}

		ImGui::Text(last_display.c_str());

		for (unsigned int i = 0; i < m_Audios.size(); i++)
		{
			if (ImGui::TreeNode(m_Audios[i]->GetName()))
			{
				if (ImGui::Button("Play"))
					PlayAudio(i);
				if (ImGui::Button("Stop"))
					StopAudio();
				ImGui::TreePop();
			}

		}
	}

}

void AudioList::GenerateImguiDisplayString()
{
	//horrible HORRIBLE way to do enum to strings
	std::string type;
	switch (GetType())
	{
	case AudioListType_Default:
		type = "TYPE - Default";
		break;
	case AudioListType_Shuffle:
		type = "TYPE - Shuffle";
		break;
	case AudioListType_WeightedRandom:
		type = "TYPE - Weighted Random";
		break;
	default:
		break;
	}

	//and another one
	std::string channel;
	switch (IsUsingPublicChannels())
	{
	case true:
		channel = "CHANNEL - Public";
		break;
	case false:
		channel = "CHANNEL - Shared";
		break;
	}

	imgui_display_string = m_Name + ": " + type + " : " + channel;
}
