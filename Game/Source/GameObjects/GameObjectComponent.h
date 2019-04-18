#pragma once

#include "../Game/PooledObject.h"
#include "GameObject3D.h"

class GameObjectComponent : public GameObject3D
{
public:
	GameObjectComponent(Scene* pScene, std::string name);
	~GameObjectComponent();

	GameObject3D* GetOwner() { return m_Owner; }
	void SetOwner(GameObject3D* owner) { m_Owner = owner; }

protected:
	GameObject3D* m_Owner;
};