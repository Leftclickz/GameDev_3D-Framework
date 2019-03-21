#pragma once

class Game;
class Mesh;
class Scene;
class Material;
class Camera;
class ResourceManager;
class BulletMotionState;
#include "Transformable.h"
#include "../Game/PooledObject.h"

class GameObject3D : public GameObject
{
	friend class BulletMotionState;
protected:
	BulletMotionState* m_MotionState;

public:
	GameObject3D(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

    virtual ~GameObject3D();

	virtual void OnEvent(Event* pEvent) override;
	virtual void Update(float deltatime)  override;
	virtual void Draw(Camera* cam) override;

	virtual void CreateBody(vec3 size, float mass = 0.0f);

	virtual void CreatePlane();

	virtual void Reset() override;

	btRigidBody* GetBody() { return m_Body; }

protected:
	bool m_WasLoadedFromJSON = false;

	btRigidBody* m_Body;
};