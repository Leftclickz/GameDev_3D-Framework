#include "GamePCH.h"
#include "AudioDataStructures.h"
#include "AudioEngine.h"
#include "AudioManager.h"

Audio::Audio(const char* audioname, const char* filename) :
	m_Source(nullptr),
	m_EventManager(nullptr),
	m_IsPlaying(false),
	m_SampleOffset(0)
{
	//Get the wave data from the resource manager
	WaveData* waveData = AudioManager::GetWaveData(filename);
	assert(waveData != nullptr);

	m_AudioName = audioname;

	//Copy the wave format and xaudio buffer
	memcpy(&m_WaveFormat, &waveData->waveFormat, sizeof(WAVEFORMATEX));
	memcpy(&m_Buffer, &waveData->buffer, sizeof(XAUDIO2_BUFFER));

	//Set the buffer's context pointer
	m_Buffer.pContext = this;

	m_CreatedVoice = false;
}

Audio::~Audio()
{
	if (m_Source != nullptr && m_CreatedVoice == true)
		AudioManager::GetEngine()->DestroyAudioVoice(m_Source);
}

void Audio::Play()
{
	Stop();

	//We can only have one
	if (IsPlaying() == false && m_Source != nullptr)
	{
		//Submit the source buffer and start playing the sound
		m_Source->SubmitSourceBuffer(&m_Buffer);
		m_Source->Start();

		//Query the state of the buffer to calculate the sample offset, 
		//this is needed for accurate playback information
		XAUDIO2_VOICE_STATE state;
		m_Source->GetState(&state);
		m_SampleOffset = state.SamplesPlayed;

		//Reset the buffer
		m_Buffer.PlayBegin = 0;

		//Set the is playing flag to true
		m_IsPlaying = true;
	}
}

void Audio::Stop()
{
	//Stop playing the sound and flush the source buffer
	if (m_Source != nullptr)
	{
		m_Source->Stop();
		m_Source->FlushSourceBuffers();

		//Set the is playing flag to false
		m_IsPlaying = false;
	}
}

bool Audio::IsPlaying()
{
	return m_IsPlaying;
}

void Audio::SetDoesLoop(bool aDoesLoop)
{
	m_Buffer.LoopCount = (aDoesLoop == true ? XAUDIO2_LOOP_INFINITE : 0);
}

bool Audio::DoesLoop()
{
	return m_Buffer.LoopCount == XAUDIO2_LOOP_INFINITE;
}

unsigned int Audio::GetNumberOfChannels()
{
	return m_WaveFormat.nChannels;
}

unsigned int Audio::GetSampleRate()
{
	return m_WaveFormat.nSamplesPerSec;
}

void Audio::SetFrequencyRatio(float aFrequencyRatio)
{
	if (m_Source != nullptr)
	{
		//Bounds check the frequency, it can't be negative
		aFrequencyRatio = fmaxf(aFrequencyRatio, 0.0f);

		//Set the frequency ratio
		m_Source->SetFrequencyRatio(aFrequencyRatio);
	}
}

float Audio::GetFrequencyRatio()
{
	if (m_Source != nullptr)
	{
		float frequencyRatio = 0;
		m_Source->GetFrequencyRatio(&frequencyRatio);
		return frequencyRatio;
	}

	return 0.0f;
}

void Audio::SetVolume(float aVolume)
{
	if (m_Source != nullptr)
		m_Source->SetVolume(aVolume);
}

float Audio::GetVolume()
{
	if (m_Source != nullptr)
	{
		float volume = 0.0f;
		m_Source->GetVolume(&volume);
		return volume;
	}

	return 0.0f;
}

void Audio::SetSample(unsigned long long aSample)
{
	//Bounds check the sample
	aSample = aSample < GetNumberOfSamples() ? aSample : GetNumberOfSamples();

	//Set the sample offset the start playing the sound at
	m_Buffer.PlayBegin = static_cast<unsigned int>(aSample);

	//If the sound is playing stop it, then start it at the need sample offset
	if (IsPlaying() == true)
	{
		Stop();
		Play();
	}
}

void Audio::SetPositionMS(unsigned int aMilleseconds)
{
	unsigned int sample = (aMilleseconds * GetSampleRate()) / 1000;
	SetSample(sample);
}

void Audio::SetPosition(double aSeconds)
{
	SetPositionMS(static_cast<unsigned int>(aSeconds * 1000.0));
}


unsigned long long Audio::GetElapsedSamples()
{
	if (m_Source != nullptr)
	{
		XAUDIO2_VOICE_STATE state;
		m_Source->GetState(&state);
		return state.SamplesPlayed - m_SampleOffset;
	}

	return 0;
}

unsigned int Audio::GetElapsedMS()
{
	//Convert samples to milleseconds
	unsigned long long samples = GetElapsedSamples();
	return static_cast<unsigned int>((samples * 1000) / GetSampleRate());
}

double Audio::GetElapsed()
{
	unsigned int milleseconds = GetElapsedMS();
	double seconds = (double)milleseconds / 1000.0;
	return seconds;
}

unsigned long long Audio::GetRemainingSamples()
{
	return GetNumberOfSamples() - GetElapsedSamples();
}

unsigned int Audio::GetRemainingMS()
{
	return GetDurationMS() - GetElapsedMS();
}

double Audio::GetRemaining()
{
	return GetDuration() - GetElapsed();
}

unsigned long long Audio::GetNumberOfSamples()
{
	//Ensure there are channels in the wave data
	if (!m_WaveFormat.nChannels)
		return 0;

	//Determine the wave format, calculate the duration accordingly
	switch (m_WaveFormat.wFormatTag)
	{
	case WAVE_FORMAT_ADPCM:
	{
		const ADPCMWAVEFORMAT* adpcmFmt = reinterpret_cast<const ADPCMWAVEFORMAT*>(&m_WaveFormat);
		uint64_t length = uint64_t(m_Buffer.AudioBytes / adpcmFmt->wfx.nBlockAlign) * adpcmFmt->wSamplesPerBlock;
		int partial = m_Buffer.AudioBytes % adpcmFmt->wfx.nBlockAlign;
		if (partial)
		{
			if (partial >= (7 * adpcmFmt->wfx.nChannels))
			{
				length += (partial * 2 / adpcmFmt->wfx.nChannels - 12);
			}
		}

		//Set the duration
		return length;
	}
	break;

	default:
	{
		if (m_WaveFormat.wBitsPerSample > 0)
		{
			return (uint64_t(m_Buffer.AudioBytes) * 8) / uint64_t(m_WaveFormat.wBitsPerSample * m_WaveFormat.nChannels);
		}
	}
	}
	return 0;
}

unsigned int Audio::GetDurationMS()
{
	//Convert samples to milleseconds
	unsigned long long samples = GetNumberOfSamples();
	return static_cast<unsigned int>((samples * 1000) / GetSampleRate());
}

double Audio::GetDuration()
{
	unsigned int milleseconds = GetDurationMS();
	double seconds = (double)milleseconds / 1000.0;
	return seconds;
}

void Audio::DispatchEvent(AudioEvent* pEvent)
{
	if (m_EventManager != nullptr)
	{
		//if we're queueing a playback stop event we are no longer playing audio
		if (pEvent->GetEventCode() == Audio_Playback_Ended)
			m_IsPlaying = false;

		m_EventManager->QueueEvent(pEvent);
	}
}

void Audio::CreateVoice()
{
	if (m_Source == nullptr)
	{
		//Create the audio voice
		AudioManager::GetEngine()->CreateAudioVoice(&m_Source, &m_WaveFormat);

		m_CreatedVoice = true;
	}
}

void Audio::SetVoice(IXAudio2SourceVoice* Voice)
{
	if (Voice == nullptr)
		return;

	if (m_Source == nullptr)
	{
		m_Source = Voice;
	}
}

