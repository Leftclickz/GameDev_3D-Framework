#pragma once

#include "Enemy.h"

//Base Enemy class. Can't take damage and deals by default the player's full health in damage
//defaulted to call TakeDamage on anything that starts contact with it
class TweenedImGuiEnemy : public Enemy
{
protected:

	float Scale_Tween_Value;
	vec3 Position_Tween_Value;
	vec3 Rotation_Tween_Value;
	TweenFunc FunctionToUse;

public:
	TweenedImGuiEnemy(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~TweenedImGuiEnemy() {}

	virtual void ImGuiDisplayDebugInfo() override;
};