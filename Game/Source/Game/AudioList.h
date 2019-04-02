#pragma once
class Audio;
class AudioVoice;

//Audio list type : used for display, casting and others.
enum AudioListType
{
	AudioListType_Default,
	AudioListType_Shuffle,
	AudioListType_WeightedRandom
};

class AudioList
{
public:

	AudioList();
	AudioList(const WAVEFORMATEX *WaveFormat);
	AudioList(AudioVoice* voice);

	virtual void Update(float deltatime);

	virtual ~AudioList();

	//Add audio to the list
	virtual void AddAudio(Audio* audio);

	//Remove audio from the list
	virtual void RemoveAudio(Audio* audio);

	//play a specific audio - doesn't affect the playback loop
	virtual void PlayAudio(unsigned int index);

	//plays a predetermined audio - affects the playback loop
	virtual void PlayAudio();

	//Flush audio buffer for this channel (if we're using a shared audio channel)
	virtual void StopAudio();

	//Set display name
	virtual void SetName(std::string name) { m_Name = name; }

	//The type of list this is. Used for IMGUI and other things.
	virtual AudioListType GetType() { return AudioListType_Default; }

	virtual void DisplayImGuiPanel();


protected:

	//is this object using public channels or does it have a shared one?
	bool IsUsingPublicChannels() { return m_Voice == nullptr; }
	void GenerateImguiDisplayString();

	AudioVoice* m_Voice = nullptr; //our channel. If left defaulted this will be null and this object will use public channels to playback audio
	std::vector<Audio*> m_Audios;
	Audio* m_LastPlayed = nullptr;

	unsigned int m_CurrentAudioToPlay = 0;
	unsigned int m_AudioPlaybackCount = 0;

	std::string m_Name;
	std::string imgui_display_string = "";

	bool isEnabled = true;//flag to enable/disable audio object entirely
};