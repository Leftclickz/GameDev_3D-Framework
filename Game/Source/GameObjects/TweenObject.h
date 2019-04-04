#pragma once
#include "../Game/TweenFuncs.h"
#include "GameObject.h"

class TweenObject
{

public:
	TweenObject(vec3& start, vec3 end, float duration, TweenFunc tweenfunction, GameObject* obj, ExitFunction exitfunction)
		: StartGoal(&start), EndGoal(end), StartSnapshot(vec3(start.x,start.y,start.z)), TimerDuration(duration), TweeningFunction(tweenfunction), Owner(obj), ExitCondition(exitfunction), ElapsedTime(0.0f) {}

	void Update(float deltatime)
	{
		ElapsedTime += deltatime;

		if (ElapsedTime >= TimerDuration)
			ElapsedTime = TimerDuration;

		StartGoal->x = TweeningFunction(StartSnapshot.x, EndGoal.x, ElapsedTime, TimerDuration);
		StartGoal->y = TweeningFunction(StartSnapshot.y, EndGoal.y, ElapsedTime, TimerDuration);
		StartGoal->z = TweeningFunction(StartSnapshot.z, EndGoal.z, ElapsedTime, TimerDuration);

		if (ElapsedTime == TimerDuration)
			(Owner->*ExitCondition) (this);
	}

protected:

	//tweening function variables
	vec3* StartGoal;
	vec3 StartSnapshot;
	vec3 EndGoal;
	TweenFunc TweeningFunction;

	//exit condition
	ExitFunction ExitCondition;
	GameObject* Owner;

	//time
	float ElapsedTime;
	float TimerDuration;
};