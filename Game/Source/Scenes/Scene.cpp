#include "GamePCH.h"

#include "../Game/Game.h"
#include "Game/ResourceManager.h"
#include "Game/BulletManager.h"
#include "Scene.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Player.h"
#include "GameObjects/PlatformerPlayer.h"
#include "GameObjects/PlatformerEnemy.h"
#include "GameObjects/Shootable.h"
#include "GameObjects/PlayerController.h"
#include "../Physics/PhysicsWorld.h"

#include "Mesh/SpriteSheet.h"
#include "GameObjects/Camera.h"
#include "../SaveLoad.h"

#include <string.h>
#include "HUD_Scene.h"

Scene::Scene(Game* pGame, ResourceManager* pResources)
{
    m_pGame = pGame;
    m_pResources = pResources;
	m_pCubePool = nullptr;
	m_Wireframe = false;
	m_DebugDraw = false;
	m_pPhysicsWorld = nullptr;
	m_ShouldReset = false;

	m_Camera = new Camera(vec3(0.0f, 5.0f, -5.0f), vec3(5.0f, 5.0f, 0.0f), 100.0f);
	m_Camera->CreateProjectionMatrix();
	m_Camera->CreateViewMatrix();
}

Scene::~Scene()
{
    for( auto pObject: m_pGameObjects )
    {
        delete pObject;
    }

	if (m_pPhysicsWorld)
	{
		delete m_pPhysicsWorld;
	}
	delete m_pCubePool;
	delete m_Camera;
}

void Scene::LoadContent()
{
}

void Scene::OnEvent(Event* pEvent)
{
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;

		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 'L')
		{
			vec3 campos = m_Camera->GetPosition();
			campos.z -= 1;
			m_Camera->SetPosition(campos);
		}
	}
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

	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
	{
		m_pGameObjects.at(i)->Draw(m_Camera);
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
}
