#ifndef __GameObject_H__
#define __GameObject_H__

class Mesh;
class Scene;
class Material;
class Camera;
class ResourceManager;
class GameObject;
class TweenObject;

#include "Transformable.h"
#include "../Game/PooledObject.h"
#include "../Game/TweenFuncs.h"

typedef void(GameObject::*ExitFunction) (TweenObject* obj);
//using ExitFunction = void (GameObject::*)(TweenObject* tween);
enum TweenType
{
	TweenType_Default,
	TweenType_Scale,
	TweenType_Rotation,
	TweenType_Translation
};

enum EDirection
{
	ELeft,
	ERight,
};

class GameObject : public Transformable
{
protected:
	std::string m_Name;

	Scene* m_pScene;

    Mesh* m_pMesh;
	Material* m_pMaterial;
	std::vector<TweenObject*> m_Tweens;

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

	virtual void AddTween(TweenType type, vec3 endgoal, float duration, TweenFunc TweenFunction);
	virtual void OnTweenEnded(TweenObject* obj);

	virtual void Reset();
};


#endif //__GameObject_H__
