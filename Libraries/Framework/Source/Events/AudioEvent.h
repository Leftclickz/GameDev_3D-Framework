#pragma once
#include "Event.h"

enum AudioEventType
{
	Audio_Playback_Started,
	Audio_Playback_Ended,
	Audio_Loop_Ended,
};

class AudioEvent : public Event 
{

public:
	AudioEvent(void* audio, AudioEventType EventCode) : m_Context(audio), m_EventCode(EventCode) {}


	virtual EventTypes GetEventType() override
	{
		return EventType_Audio;
	}


	void* GetContext() { return m_Context; }
	AudioEventType GetEventCode() { return m_EventCode; }

protected:

	void* m_Context;
	AudioEventType m_EventCode;
};