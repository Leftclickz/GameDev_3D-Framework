#include "GamePCH.h"
#include "AudioDataStructures.h"
#include "AudioEngine.h"
#include "AudioManager.h"
#include "AudioVoice.h"

Audio::Audio(const char* audioname, const char* filename) :
	m_Voice(nullptr),
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
}

Audio::~Audio()
{
	if (m_Voice != nullptr)
	{
		if (m_Voice->GetVoiceType() == VoiceType_Dedicated)
			AudioManager::GetEngine()->DestroyAudioVoice(m_Voice);
	}
}

void Audio::Play(bool UsePublicChannel /*= false*/)
{

	if (UsePublicChannel == false)
	{
		if (m_Voice != nullptr)
		{
			Stop();
			m_Voice->PlayAudio(this);
			m_IsPlaying = true;
		}
	}
	else
	{
		AudioVoice* channel = AudioManager::GetEngine()->GetAvailablePublicChannel();
		if (channel != nullptr)
		{
			channel->PlayAudio(this);
		}
	}

}

void Audio::Stop()
{
	if (m_Voice != nullptr)
	{
		m_Voice->StopAudio();
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
	if (m_Voice != nullptr)
	{
		//Bounds check the frequency, it can't be negative
		aFrequencyRatio = fmaxf(aFrequencyRatio, 0.0f);

		//Set the frequency ratio
		m_Voice->GetSource()->SetFrequencyRatio(aFrequencyRatio);
	}
}

float Audio::GetFrequencyRatio()
{
	if (m_Voice != nullptr)
	{
		float frequencyRatio = 0;
		m_Voice->GetSource()->GetFrequencyRatio(&frequencyRatio);
		return frequencyRatio;
	}

	return 0.0f;
}

void Audio::SetVolume(float aVolume)
{
	if (m_Voice != nullptr)
		m_Voice->GetSource()->SetVolume(aVolume);
}

float Audio::GetVolume()
{
	if (m_Voice != nullptr)
	{
		float volume = 0.0f;
		m_Voice->GetSource()->GetVolume(&volume);
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
	if (m_Voice != nullptr)
	{
		XAUDIO2_VOICE_STATE state;
		m_Voice->GetSource()->GetState(&state);
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
		m_EventManager->QueueEvent(pEvent);
	}
}

void Audio::CreateVoice()
{
	if (m_Voice == nullptr)
	{
		m_Voice = new AudioVoice(m_WaveFormat, VoiceType_Dedicated);

		//Create the audio voice
		AudioManager::GetEngine()->CreateAudioVoice(m_Voice);
	}
}

void Audio::SetVoice(AudioVoice* Voice)
{
	if (Voice == nullptr)
		return;

	if (m_Voice == nullptr)
	{
		m_Voice = Voice;
	}
}

