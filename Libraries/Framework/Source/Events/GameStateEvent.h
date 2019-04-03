#pragma once

enum EGameState
{
	TitleState,
	PlayState,
	PauseState,
	WinState,
	LoseState

};

class GameStateEvent : public Event {

public:
	GameStateEvent(EGameState state) : GameState(state) {}
	virtual EventTypes GetEventType() override { return EventType_GameState; }

	EGameState GetGameState() { return GameState; }

protected:

	EGameState GameState;
};