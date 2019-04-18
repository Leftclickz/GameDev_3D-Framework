#pragma once

#include "../Game/PooledObject.h"
#include "GameObject3D.h"

class BoshyBullet : public GameObject3D, public PooledObject<BoshyBullet*>
{
	const float LIFETIME = 5.0f;

public:
	BoshyBullet(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial, Pool<BoshyBullet*>* pool);
	~BoshyBullet();

	void SetSpeed(float speed) { m_Speed = speed; }
	void SetDamage(float damage) { m_Damage = damage; }

	virtual void Update(float delta) override;
	virtual void Draw(Camera* cam) override;

	class Gun* GetOwner() { return m_Owner; }
	void SetOwner(class Gun* owner) { m_Owner = owner; }

	virtual void ContactStarted(GameObject3D* pOtherObj, vec3 normal) override;

	void Fire();

protected:
	float m_Speed;
	float m_Damage;

	float m_LifeTime;

	class Gun* m_Owner;

	void Deactivate();
	void Activate();
};