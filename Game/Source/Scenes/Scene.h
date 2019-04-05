#ifndef __Scene_H__
#define __Scene_H__

class Game;
class GameObject;
class ResourceManager;
class BulletManager;
class SceneManager;
class PhysicsWorld;
class Camera;
class HUD_Scene;

#include "../Game/Pool.h"
#include "../GameObjects/GameObject.h"
#include "Game/SceneManager.h"
#include "Game/Game.h"
#include "Game/ResourceManager.h"
#include "Game/AudioManager.h"
#include "GameObjects/Camera.h"

class Scene
{
protected:
    Game* m_pGame;
    ResourceManager* m_pResources;
	SceneManager* m_pSceneManager;

    std::vector<GameObject*> m_pGameObjects;
	std::vector<LightObject*> m_pLights;
    std::vector<btRigidBody*> m_pBodies3D;

	PhysicsWorld* m_pPhysicsWorld;

	Pool<GameObject*>* m_pCubePool;

	bool m_Wireframe;
	bool m_DebugDraw;
	bool m_ShouldFilter = false;

	Camera* m_Camera;

	Pool<GameObject*> m_RemovedPool;
	BulletManager* m_BulletManager;

	std::string m_Name = "Scene";

	bool m_IsTransparent = false;
	bool m_DoesPause = false;
	bool m_DoesStopEvents = false;

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

	bool AddLightObject(LightObject* pObject);
	bool RemoveLightObject(LightObject* pObject);
	void TempRemoveLightObject(LightObject* pObj);
	bool HideLightObject(LightObject * pObject);
	LightObject* GetLightObjectByName(std::string name);

	virtual std::vector<LightObject*>* GetLightVector() { return &m_pLights; }

	void Add3DBody(btRigidBody* pObject);
	bool Remove3DBody(btRigidBody* pObject);

    Game* GetGame() { return m_pGame; }
    ResourceManager* GetResourceManager() { return m_pResources; }
	PhysicsWorld* GetPhysicsWorld();
	b2World* Getb2World();

	std::string GetName() { return m_Name; }

	void SetSceneManager(SceneManager* sm) { m_pSceneManager = sm; }
	SceneManager* GetSceneManager() { return m_pSceneManager; }

	virtual void Reset();

	BulletManager* GetBulletManager() { return m_BulletManager; }

	//if other scenes behind this can be seen through it
	bool IsTransparent() { return m_IsTransparent; }
	//if scenes under this should be allowed to update
	bool DoesPause() { return m_DoesPause; }
	//if this scene stops scenes below it from getting events
	bool DoesStopEvents() { return m_DoesStopEvents; }

	//event-driven function for when this scene is no longer the current scene for the game
	virtual void HasLeftFocus() {}

	//event-driven function for when this scene is now the current scene for the game
	virtual void HasEnteredFocus() {}

};

#endif //__Scene_H__
