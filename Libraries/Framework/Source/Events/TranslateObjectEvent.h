#pragma once
#include "Event.h"

class TranslateObjectEvent : public Event
{
protected:

public:
	TranslateObjectEvent(float x, float y, float z, void* ptr) { X = x; Y = y; Z = z; Object = ptr; }

	virtual EventTypes GetEventType() override { return EventType_TranslateObject; }

	void GetPosition(float &x, float &y, float &z) { x = X; y = Y; z = Z; }

	void* GetTranslatingObject() { return Object; }

protected:
	float X;
	float Y;
	float Z;

	void* Object;
};