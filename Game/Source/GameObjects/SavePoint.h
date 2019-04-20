#pragma once

#include "GameObjects/GameObject3D.h"

//Base Enemy class. Can't take damage and deals by default the player's full health in damage
//defaulted to call TakeDamage on anything that starts contact with it
class SavePoint : public GameObject3D
{
protected:
	std::map<std::string, class AudioList*> m_PlayerSounds;

public:
	SavePoint(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~SavePoint();

	virtual void ContactStarted(GameObject3D* pOtherObj, vec3 normal) override;
	virtual void ContactEnded(GameObject3D* pOtherObj) override;

	virtual void SetRespawnValues();
};