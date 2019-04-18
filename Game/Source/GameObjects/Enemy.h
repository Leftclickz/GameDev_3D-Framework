#pragma once

#include "GameObjects/GameObject3D.h"

//Base Enemy class. Can't take damage and deals by default the player's full health in damage
//defaulted to call TakeDamage on anything that starts contact with it
class Enemy : public GameObject3D
{
protected:
	float m_Speed;
	float m_Health;
	float m_Damage;

	std::map<std::string, class AudioList*> m_PlayerSounds;

public:
	Enemy(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~Enemy();

	virtual void Update(float deltatime) override;

	void SetSpeed(float speed) { m_Speed = speed; }
	void SetHealth(float health) { m_Health = health; }
	void SetDamage(float damage) { m_Damage = damage; }

	virtual void ContactStarted(GameObject3D* pOtherObj, vec3 normal) override;
	virtual void ContactEnded(GameObject3D* pOtherObj) override;
};