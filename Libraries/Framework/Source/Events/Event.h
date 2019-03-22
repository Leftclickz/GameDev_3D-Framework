#ifndef __Event_H__
#define __Event_H__

enum EventTypes
{
    EventType_Input,
	EventType_HUDValueAdjust,
	EventType_TranslateObject,
	EventType_GameState,
	EventType_Audio,
	EventType_SceneChange
};

class Event
{
protected:

public:
    Event() {}
    virtual ~Event() {}

    virtual EventTypes GetEventType() = 0;
};

#endif //__Event_H__
