#pragma once
#include "../Framework/Source/Events/AudioEvent.h"

#define EVENTCALLBACK __declspec(nothrow) void __stdcall

class AudioVoice;

struct WaveData
{
	WaveData()
	{
		//allocate 0 memory for buffer and waveformat
		ZeroMemory(&waveFormat, sizeof(waveFormat));
		ZeroMemory(&buffer, sizeof(buffer));

		//our raw audio chunk
		data = nullptr;
	}

	~WaveData()
	{
		delete[] data;
	}

	WAVEFORMATEX waveFormat;
	XAUDIO2_BUFFER buffer;
	void* data;

	const char* name;
};


//Audio class to handle playback of both music and sounds effects in game.
class Audio
{
public:
	//Constructor for the Audio class, many extensions are supported, use 'streamed' for longer files such as background music.
	//Don't stream for short files, such as sound effects. Audio files be looped.
	Audio(const char* audioname, const char* filename);
	~Audio();

	//Play and Stop methods
	void Play(bool UsePublicChannel = false);
	void Stop();

	//Returns wether the audio is playing
	bool IsPlaying();

	//Sets the audio file loops or not
	void SetDoesLoop(bool doesLoop);

	//Returns the audio file loops or not
	bool DoesLoop();

	//Returns the number of channels 
	unsigned int GetNumberOfChannels();

	//Returns the sample rate
	unsigned int GetSampleRate();

	//Set the frequency of the audio file, must be positive
	void SetFrequencyRatio(float frequencyRatio);

	//Returns the current frequency of the audio file
	float GetFrequencyRatio();

	//Set the volume of the audio file, range 0.0f to 1.0f
	void SetVolume(float volume);

	//Returns the current volume of the audio file
	float GetVolume();

	//Sets the current position of the audio file, in samples
	void SetSample(unsigned long long sample);

	//Sets the current position of the audio file, in milleseconds
	void SetPositionMS(unsigned int milleseconds);

	//Sets the current position of the audio file, in seconds
	void SetPosition(double seconds);

	//Returns the current time of the audio file, in samples
	unsigned long long GetElapsedSamples();

	//Returns the current time of the audio file, in milleseconds
	unsigned int GetElapsedMS();

	//Returns the current time of the audio file, in seconds
	double GetElapsed();

	//Returns the remaining time of the audio file, in samples
	unsigned long long GetRemainingSamples();

	//Returns the remaining time of the audio file, in milleseconds
	unsigned int GetRemainingMS();

	//Returns the remaining time of the audio file, in seconds
	double GetRemaining();

	//Returns the number of samples of the audio object
	unsigned long long GetNumberOfSamples();

	//Returns the duration of the audio object, in milleseconds
	unsigned int GetDurationMS();

	//Returns the duration of the audio object, in seconds
	double GetDuration();

	//get the name of this audio object
	const char* GetName() { return m_AudioName; }

	//set the event manager here, without this events will not be fired.
	void SetEventManager(class EventManager* manager) { m_EventManager = manager; }

	//support function for dispatching audio events to the event manager
	void DispatchEvent(AudioEvent* pEvent);
	
	//creates a unique voice channel for the audio class
	void CreateVoice();

	//sets the voice for this audio
	void SetVoice(AudioVoice* Voice);

	//Get the wave format of this audio file
	WAVEFORMATEX* GetWaveFormat() { return &m_WaveFormat; }

	XAUDIO2_BUFFER* GetBuffer() { return &m_Buffer; }
	void SetSampleOffset(unsigned long long Offset) { m_SampleOffset = Offset; }

protected:
	//Member variables
	AudioVoice* m_Voice;
	WAVEFORMATEX m_WaveFormat;
	XAUDIO2_BUFFER m_Buffer;
	bool m_IsPlaying;
	unsigned long long m_SampleOffset;
	const char* m_AudioName;
	EventManager* m_EventManager;
};


//wrapper xaudiocallback class for supporting audio callback events.
struct AudioEventCallback : public IXAudio2VoiceCallback
{
	AudioEventCallback()
	{
	}

	virtual ~AudioEventCallback()
	{
	}

	//declared but not implemented (if they arent there then the class is considered abstract)
	EVENTCALLBACK OnVoiceProcessingPassStart(UINT32 buffer) {}
	EVENTCALLBACK OnVoiceProcessingPassEnd() {}
	EVENTCALLBACK OnStreamEnd() {}
	EVENTCALLBACK OnVoiceError(void* context, HRESULT) {}

	//event call for when an audio class starts playing
	EVENTCALLBACK OnBufferStart(void* context)
	{
		if (context != nullptr)
		{
			Audio* audio = reinterpret_cast<Audio*>(context);
			if (audio) audio->DispatchEvent(new AudioEvent(audio, Audio_Playback_Started));
		}
	}

	//event call for when an audio class finishes playing
	EVENTCALLBACK OnBufferEnd(void* context)
	{
		if (context != nullptr)
		{
			Audio* audio = reinterpret_cast<Audio*>(context);
			if (audio) audio->DispatchEvent(new AudioEvent(audio, Audio_Playback_Ended));
		}
	}

	//event call for when an audio class completes a loop
	EVENTCALLBACK OnLoopEnd(void* context)
	{
		if (context != nullptr)
		{
			Audio* audio = reinterpret_cast<Audio*>(context);
			if (audio) audio->DispatchEvent(new AudioEvent(audio, Audio_Loop_Ended));
		}
	}

};