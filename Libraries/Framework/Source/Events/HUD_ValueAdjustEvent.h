#pragma once
#include "Event.h"

enum ValueToAdjust
{
	Value_Score,
	Value_Life
};

class HUD_ValueAdjustEvent : public Event
{
protected:

public:
	HUD_ValueAdjustEvent(ValueToAdjust type, int value) : m_Type(type), m_Value(value) {}

	virtual EventTypes GetEventType() override { return EventType_HUDValueAdjust; }

	ValueToAdjust GetAdjustType() { return m_Type; }
	int GetValue() { return m_Value; }

protected:

	int m_Value;
	ValueToAdjust m_Type;
};