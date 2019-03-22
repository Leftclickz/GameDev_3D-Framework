#pragma once
#include "Event.h"

class SceneChangeEvent : public Event {

public:

	SceneChangeEvent(void* SceneFrom, void* SceneTo) : m_SceneFrom(SceneFrom), m_SceneTo(SceneTo) {}


	virtual EventTypes GetEventType() override
	{
		return EventType_SceneChange;
	}

	void* GetFrom() { return m_SceneFrom; }
	void* GetTo() { return m_SceneTo; }

protected:

	void* m_SceneFrom;
	void* m_SceneTo;
};