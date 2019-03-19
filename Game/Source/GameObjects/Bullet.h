#pragma once
#include "GameObjects/GameObject2D.h"

class Bullet : public GameObject2D
{
public:
	Bullet(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~Bullet();

	virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;

	void SetPool(Pool<GameObject*>* pPool);

	void Fire(vec3 pos, EDirection dir);

	virtual void BeginContact(b2Fixture* fix, vec2 pos, vec2 normal) override;
	virtual void EndContact(GameObject2D* pobj) override;

	void RemoveSelf();
protected:
	Pool<GameObject*>* m_pPoolThisObjectCameFrom;

};