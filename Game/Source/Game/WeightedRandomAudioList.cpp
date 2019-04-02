#include "GamePCH.h"
#include "WeightedRandomAudioList.h"
#include "AudioDataStructures.h"



void WeightedRandomAudioList::AddAudio(Audio* audio)
{
	AddAudio(audio, 10);
}

void WeightedRandomAudioList::AddAudio(Audio* audio, int value)
{
	AudioList::AddAudio(audio);

	m_AudioWeights.push_back(value);
}

void WeightedRandomAudioList::RemoveAudio(Audio* audio)
{
	auto iteratorForObject = std::find(m_Audios.begin(), m_Audios.end(), audio);

	//remove the object if it exists
	if (iteratorForObject != m_Audios.end())
	{
		// Get index of element from iterator
		int index = std::distance(m_Audios.begin(), iteratorForObject);

		//remove the elements
		m_Audios.erase(iteratorForObject);
		m_AudioWeights.erase(m_AudioWeights.begin() + index);
	}

	m_MostRecentlyPlayedIndex = UINT32_MAX;
}

void WeightedRandomAudioList::SetAudioWeightValue(Audio* audio, int value)
{
	auto iteratorForObject = std::find(m_Audios.begin(), m_Audios.end(), audio);

	if (iteratorForObject != m_Audios.end())
	{
		// Get index of element from iterator
		int index = std::distance(m_Audios.begin(), iteratorForObject);

		//add the value
		m_AudioWeights[index] = value;
	}
}

void WeightedRandomAudioList::PlayAudio()
{
	//get our total weight
	int max = GetWeightCount();

	bool complete = false;
	while (complete == false)
	{
		//get a random number and start at 0
		int random_num = rand() % max + 1;
		int count = 0;

		for (unsigned int i = 0; i < m_AudioWeights.size(); i++)
		{
			//compare against our weight to check which lucky bastard we got
			if (random_num >= count && random_num - count <= m_AudioWeights[i])
			{
				//if we played this 1, redo the random loop
				if (m_MostRecentlyPlayedIndex == i)
					break;
				//otherwise clean up and get out
				else
				{
					m_MostRecentlyPlayedIndex = i;

					//play the audio and get out
					m_Audios[i]->Play(IsUsingPublicChannels());
					m_LastPlayed = m_Audios[i];
					complete = true;
					break;
				}
			}
			else
			{
				//add our weight to the counter
				count += m_AudioWeights[i];
			}
		}
	}
}

void WeightedRandomAudioList::DisplayImGuiPanel()
{
	ImGui::Begin("Sound");
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(imgui_display_string.c_str()))
	{
		ImGui::Checkbox("Enabled", &isEnabled);

		if (isEnabled == true)
		{
			if (ImGui::Button("Play Next"))
				PlayAudio();

			std::string last_display;
			if (m_LastPlayed != nullptr)
				last_display = "Last Audio Played: " + std::string(m_LastPlayed->GetName());
			else
				last_display = "Last Audio Played: NONE";

			ImGui::Text(last_display.c_str());

			for (unsigned int i = 0; i < m_Audios.size(); i++)
			{

				std::string display = std::string(m_Audios[i]->GetName()) + " - Weight - " + std::to_string(m_AudioWeights[i]);

				if (ImGui::TreeNode(display.c_str()))
				{
					if (ImGui::Button("Play"))
						AudioList::PlayAudio(i);
					if (ImGui::Button("Stop"))
						StopAudio();
					ImGui::SliderInt("Weight", &m_AudioWeights[i], 0, 100);
					ImGui::TreePop();
				}

			}
		}
	}

	ImGui::PopID();
	ImGui::End();
}

int WeightedRandomAudioList::GetWeightCount()
{
	int value = 0;
	for (unsigned int i = 0; i < m_AudioWeights.size(); i++)
		value += m_AudioWeights[i];

	return value;
}
