#include "GamePCH.h"
#include "Scene.h"

#include "Game/BulletManager.h"

#include "GameObjects/Player.h"
#include "GameObjects/PlatformerPlayer.h"
#include "GameObjects/PlatformerEnemy.h"
#include "GameObjects/Shootable.h"
#include "GameObjects/PlayerController.h"
#include "../Physics/PhysicsWorld.h"

#include "Mesh/SpriteSheet.h"
#include "../SaveLoad.h"

#include <string.h>
#include "HUD_Scene.h"
#include "GameObjects/LightObject.h"

Scene::Scene(Game* pGame, ResourceManager* pResources)
{
    m_pGame = pGame;
    m_pResources = pResources;
	m_pCubePool = nullptr;
	m_Wireframe = false;
	m_DebugDraw = false;
	m_pPhysicsWorld = nullptr;
	m_ShouldReset = false;
	m_BulletManager = nullptr;

	m_Camera = new Camera(this, "Camera", Transform(vec3(0.0f, 5.0f, -5.0f)));
	AddGameObject(m_Camera);
}

Scene::~Scene()
{
    for( auto pObject: m_pGameObjects )
    {
        delete pObject;
    }

	for (auto pObject : m_pLights)
	{
		delete pObject;
	}

	if (m_pPhysicsWorld)
	{
		delete m_pPhysicsWorld;
	}
	delete m_pCubePool;

	if (m_BulletManager)
	{
		delete m_BulletManager;
	}
}

void Scene::LoadContent()
{
}

void Scene::OnEvent(Event* pEvent)
{
}

void Scene::Update(float deltatime)
{
	//basically used for delayed resetting instead of doing it at will.
	if (m_ShouldReset)
	{
		Reset();
		m_ShouldReset = false;
	}

	m_Camera->Update(deltatime);

	if (m_pPhysicsWorld)
	{
		m_pPhysicsWorld->Update(deltatime);
	}

	for (unsigned int i = 0; i < m_pLights.size(); i++)
	{
		m_pLights.at(i)->Update(deltatime);
	}

	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
	{
		m_pGameObjects.at(i)->Update(deltatime);
	}
}

void Scene::Draw()
{
	if (m_Wireframe)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	for (unsigned int i = 0; i < m_pLights.size(); i++)
	{
		m_pLights.at(i)->Draw(m_Camera);
	}

	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
	{
		if (m_pGameObjects[i]->GetEnabled())
		{
			m_pGameObjects.at(i)->Draw(m_Camera);
		}
	}

	if (m_DebugDraw)
	{
		if (m_pPhysicsWorld)
		{
			m_pPhysicsWorld->Draw(m_Camera, m_pResources->GetMaterial("Debug"));
		}
	}
}

void Scene::LoadFromSceneFile(std::string filename)
{
	
}

void Scene::AddGameObject(GameObject * pObject)
{
	m_pGameObjects.push_back(pObject);
}

bool Scene::RemoveGameObject(GameObject * pObject)
{
	auto iteratorForObject = std::find(m_pGameObjects.begin(), m_pGameObjects.end(), pObject);

	if (iteratorForObject != m_pGameObjects.end())
	{
		m_pGameObjects.erase(iteratorForObject);
		return true;
	}

	return false;
}

//removes the object from the scene but adds it to a removed pool so it isn't just dropped
void Scene::TempRemoveGameObject(GameObject* pObj)
{
	auto iteratorForObject = std::find(m_pGameObjects.begin(), m_pGameObjects.end(), pObj);

	if (iteratorForObject != m_pGameObjects.end())
	{
		m_RemovedPool.AddObjectToPool(pObj);
		m_pGameObjects.erase(iteratorForObject);
	}
}

bool Scene::HideGameObject(GameObject * pObject)
{
	auto iteratorForObject = std::find(m_pGameObjects.begin(), m_pGameObjects.end(), pObject);

	if (iteratorForObject != m_pGameObjects.end())
	{
		unsigned int index = std::distance(m_pGameObjects.begin(), iteratorForObject);

		if (index < m_pGameObjects.size())
		{
			m_pGameObjects.at(index)->SetEnabled(false);
			return true;
		}
		return false;
	}
	return false;
}

GameObject * Scene::GetGameObjectByName(std::string name)
{
	for (auto pObject : m_pGameObjects)
	{
		if (pObject->GetName() == name)
		{
			return pObject;
		}
	}

	return nullptr;
}


bool Scene::AddLightObject(LightObject * pObject)
{
	auto iteratorForObject = std::find(m_pLights.begin(), m_pLights.end(), pObject);

	if (iteratorForObject == m_pLights.end())
	{
		m_pLights.push_back(pObject);
		return true;
	}
	return false;
}

bool Scene::RemoveLightObject(LightObject * pObject)
{
	auto iteratorForObject = std::find(m_pLights.begin(), m_pLights.end(), pObject);

	if (iteratorForObject != m_pLights.end())
	{
		m_pLights.erase(iteratorForObject);
		return true;
	}

	return false;
}

void Scene::TempRemoveLightObject(LightObject * pObj)
{
	auto iteratorForObject = std::find(m_pLights.begin(), m_pLights.end(), pObj);

	if (iteratorForObject != m_pLights.end())
	{
		m_RemovedPool.AddObjectToPool(pObj);
		m_pLights.erase(iteratorForObject);
	}
}

bool Scene::HideLightObject(LightObject * pObject)
{
	auto iteratorForObject = std::find(m_pLights.begin(), m_pLights.end(), pObject);

	if (iteratorForObject != m_pLights.end())
	{
		unsigned int index = std::distance(m_pLights.begin(), iteratorForObject);

		if (index < m_pLights.size())
		{
			m_pLights.at(index)->SetEnabled(false);
			return true;
		}
		return false;
	}
	return false;
}

LightObject * Scene::GetLightObjectByName(std::string name)
{
	for (auto pObject : m_pLights)
	{
		if (pObject->GetName() == name)
		{
			return pObject;
		}
	}
	return nullptr;
}

std::vector<LightObject*> Scene::GetClosestLights(vec3 pos)
{
	if (m_pLights.size() <= 5)
	{
		return m_pLights;
	}

	struct Lights
	{
		float Dist;
		LightObject* Light;
		Lights(float dist, LightObject* light) { Dist = dist, Light = light; }
	};

	struct less_than_key
	{
		inline bool operator() (const Lights& struct1, const Lights& struct2)
		{
			return (struct1.Dist < struct2.Dist);
		}
	};

	std::vector<LightObject*> closeLights;
	std::vector<Lights> allLights;

	//fill it with every light and their matching distance
	for (auto i : m_pLights)
	{
		vec3 diff = pos - i->GetPosition();
		float length = diff.LengthSquared();

		allLights.push_back(Lights(length, i));
	}

	std::sort(allLights.begin(), allLights.end(), less_than_key());

	for (unsigned int i = 0; i < 5; i++)
	{
		closeLights.push_back(allLights.at(i).Light);
	}
	return closeLights;
}

void Scene::Add3DBody(btRigidBody* pObject)
{
	m_pBodies3D.push_back(pObject);
}

bool Scene::Remove3DBody(btRigidBody* pObject)
{
	auto iteratorForObject = std::find(m_pBodies3D.begin(), m_pBodies3D.end(), pObject);

	if (iteratorForObject != m_pBodies3D.end())
	{
		m_pBodies3D.erase(iteratorForObject);
		return true;
	}

	return false;
}

PhysicsWorld* Scene::GetPhysicsWorld()
{
	if (m_pPhysicsWorld == nullptr)
	{
		m_pPhysicsWorld = new PhysicsWorld();
		m_pPhysicsWorld->Init(b2Vec2(0.0f, -9.8f));
	}
	return m_pPhysicsWorld;
}

b2World* Scene::Getb2World()
{
	if (m_pPhysicsWorld == nullptr)
	{
		m_pPhysicsWorld = new PhysicsWorld();
		m_pPhysicsWorld->Init(b2Vec2(0.0f, -9.8f));
	}
	return m_pPhysicsWorld->GetWorld();
}

void Scene::Reset()
{
	//add anything that was removed temporarily back into the scene
	for (unsigned int j = 0; j < m_RemovedPool.GetSizeOfPool(); j++)
	{
		GameObject* pObj = m_RemovedPool.GetObjectFromPool();

		if (pObj)
		{
			AddGameObject(pObj);
		}
		else
			break;
	}

	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
	{
		m_pGameObjects[i]->Reset();
		m_pGameObjects[i]->SetEnabled(true);
	}

	for (unsigned int i = 0; i < m_pLights.size(); i++)
	{
		m_pLights[i]->Reset();
		m_pLights[i]->SetEnabled(true);
	}

	m_Camera->Reset();
}

void Scene::ImGuiDisplayDebugData()
{
	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
		m_pGameObjects[i]->ImGuiDisplayDebugInfo();

	for (unsigned int i = 0; i < m_pLights.size(); i++)
		m_pLights[i]->ImGuiDisplayDebugInfo();
}
