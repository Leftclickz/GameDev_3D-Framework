#pragma once

class Game;
class Mesh;
class Scene;
class Material;
class Camera;
class ResourceManager;
#include "Transformable.h"
#include "../Game/PooledObject.h"

class GameObject2D : public GameObject
{
protected:
	b2Body* m_pBody;

public:
	GameObject2D(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

    virtual ~GameObject2D();

	virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;

    // Setters
	b2Body* GetBody();

	virtual void CreateBody(bool isStatic);
	virtual void CreateRevolute(b2Body* pOtherBody, bool isMotorEnabled, float motorSpeed, float motorTorque);
	virtual void CreatePrismatic(b2Body* pOtherBody, b2Vec2 axis, bool isMotorEnabled, float motorSpeed, float motorMaxForce, bool isLimitEnabled, float LimitMin, float LimitMax);

	void AddBox(vec2 size, float32 density = 1.0f, b2Vec2 offset = b2Vec2(0.0f, 0.0f), bool isSensor = false, float32 friction = 0.7f, float32 restitution = 0.0f);
	void AddBox(cJSON* jObj);
	void AddCircle(float32 radius, b2Vec2 offset = b2Vec2(0.0f, 0.0f), float32 density = 1.0f, bool isSensor = false, float32 friction = 0.7f, float32 restitution = 0.0f);
	void AddCircle(cJSON* jObj);
	void AddRevoluteJoint(cJSON* jObj);
	void AddPrismaticJoint(cJSON* jObj);
	void AddWeldJoint(cJSON* jObj);
	void AddEdge();

	virtual void BeginContact(b2Fixture* fix, vec2 pos, vec2 normal);
	virtual void EndContact(GameObject2D* pobj);

	virtual void LoadFromcJSON(cJSON* obj, ResourceManager* manager);
	virtual void LoadJointsFromcJSON(cJSON* obj);
	virtual void HandleJoints(cJSON* obj);

	virtual void Reset() override;

private:
	vec3 cJSONpos;
	vec3 cJSONrot;
	vec3 cJSONscale;

	bool m_WasLoadedFromJSON = false;
};