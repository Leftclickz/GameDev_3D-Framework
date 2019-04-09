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
public:
	GameObject3D(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~GameObject3D();

	virtual void OnEvent(Event* pEvent) override;
	virtual void Update(float deltatime)  override;
	virtual void Draw(Camera* cam) override;

	virtual void CreateBoxBody(vec3 size, float mass = 0.0f);
	virtual void CreateSphereBody(float radius, float mass = 0.0f);
	virtual void CreateConvexHullBody(float mass = 0.0f);

	virtual void CreatePlane(vec3 normal = vec3(0, 1, 0), float planeconatant = 0.0f);

	virtual void Reset() override;

	btRigidBody* GetBody() { return m_Body; }

	virtual void ContactStarted(GameObject3D* pOtherObj, vec3 normal);
	virtual void ContactEnded(GameObject3D* pOtherObj);

	virtual void LoadFromcJSON(cJSON* obj, ResourceManager* manager) override;
	virtual void HandleComponents(cJSON* obj, ResourceManager* manager);
	virtual void HandleMeshLoad(cJSON* obj, const char* type, ResourceManager* manager);
	virtual void HandleCollisionLoad(cJSON* obj);

protected:
	virtual void CreateBody(btCollisionShape* shape, float mass);
	virtual void DisplayImguiDebugInfo();

	BulletMotionState* m_MotionState;
	btRigidBody* m_Body;

	bool m_WasLoadedFromJSON = false;

private:

	vec3 cJSONpos;
	vec3 cJSONrot;
	vec3 cJSONscale;	
};