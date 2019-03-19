#pragma once
#include "GameObjects/GameObject2D.h"

class Shootable : public GameObject2D
{
public:
	Shootable(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~Shootable();

	virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;

	virtual void BeginContact(b2Fixture* fix, vec2 pos, vec2 normal) override;
	virtual void EndContact(GameObject2D* pobj) override;
};