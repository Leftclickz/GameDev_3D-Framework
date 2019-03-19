#ifndef __Scene_H__
#define __Scene_H__

class Game;
class GameObject;
class ResourceManager;
class BulletManager;
class PhysicsWorld;
class Camera;
class HUD_Scene;

#include "../Game/Pool.h"

class Scene
{
protected:
    Game* m_pGame;
    ResourceManager* m_pResources;

    std::vector<GameObject*> m_pGameObjects;

	PhysicsWorld* m_pPhysicsWorld;

	Pool<GameObject*>* m_pCubePool;

	bool m_Wireframe;
	bool m_DebugDraw;
	bool m_ShouldFilter = false;

	Camera* m_Camera;

	Pool<GameObject*> m_RemovedPool;

	BulletManager* m_BulletManager;

public:
	bool m_ShouldReset = false;

    Scene(Game* pGame, ResourceManager* pResources);
    virtual ~Scene();

    virtual void LoadContent();

    virtual void OnEvent(Event* pEvent);
    virtual void Update(float deltatime);
	virtual void Draw();

	virtual void LoadFromSceneFile(std::string filename);

	void AddGameObject(GameObject* pObject);
	bool RemoveGameObject(GameObject* pObject);
	void TempRemoveGameObject(GameObject* pObj);
	bool HideGameObject(GameObject * pObject);
	GameObject* GetGameObjectByName(std::string name);

    Game* GetGame() { return m_pGame; }
    ResourceManager* GetResourceManager() { return m_pResources; }
	PhysicsWorld* GetPhysicsWorld();
	b2World* Getb2World();

	virtual void Reset();

	BulletManager* GetBulletManager() { return m_BulletManager; }
};

#endif //__Scene_H__
