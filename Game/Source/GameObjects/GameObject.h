#ifndef __GameObject_H__
#define __GameObject_H__

class Game;
class Mesh;
class Scene;
class Material;
class Camera;
class ResourceManager;
#include "Transformable.h"
#include "../Game/PooledObject.h"

enum EDirection
{
	ELeft,
	ERight,
};

class GameObject : public Transformable
{
protected:
    Game* m_pGame;

	Scene* m_pScene;

    Mesh* m_pMesh;
	Material* m_pMaterial;

	std::string m_Name;

	bool isEnabled = true;

public:
	GameObject(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

    virtual ~GameObject();

    virtual void OnEvent(Event* pEvent) {}
	virtual void Update(float deltatime);
	virtual void Draw(Camera* cam);

	Mesh* GetMesh() { return m_pMesh; }
	Material* GetMaterial() { return m_pMaterial; }
	std::string GetName() { return m_Name; }

    // Setters
    void SetMesh(Mesh* pMesh) { m_pMesh = pMesh; }
    void SetMaterial(Material* pMat) { m_pMaterial = pMat;}

	void SetEnabled(bool enabled) { isEnabled = enabled; }
	bool GetEnabled() { return isEnabled; }

	virtual void LoadFromcJSON(cJSON* obj, ResourceManager* manager) {}

	virtual void Reset();
};

#endif //__GameObject_H__
