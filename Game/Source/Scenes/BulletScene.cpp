#include "GamePCH.h"
#include "BulletScene.h"

#include "../Game/Game.h"
#include "Game/ResourceManager.h"
#include "Scene.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Player.h"
#include "GameObjects/PlayerController.h"
#include "../Physics/PhysicsWorld.h"
#include "Mesh/SpriteSheet.h"

#include "DebugDraw.h"
#include "GameObjects/Camera.h"
#include "GameObjects/ChaseCameraObject.h"
#include "Game/BulletManager.h"

BulletScene::BulletScene(Game* pGame, ResourceManager* pResources) :
	Scene(pGame, pResources)
{
	//remove the standard cam
	GameObject* cam = GetGameObjectByName("Camera");
	delete cam;
	RemoveGameObject(cam);

	//create the proper chase-cam
	m_Camera = new ChaseCameraObject(this, "ChaseCamera", Transform(vec3(0.0f, 10.0f, -15.0f)));
	AddGameObject(m_Camera);

	m_BulletManager = new BulletManager();
}

BulletScene::~BulletScene()
{
}

void BulletScene::LoadContent()
{
	Scene::LoadContent();
	m_pResources->AddTexture("White", new Texture("Data/Textures/White.png"));

	m_pResources->AddMaterial("Lighting", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("Water")));
	m_pResources->AddMaterial("Lighting2", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("White")));

	Player* player = new Player(this, "Player", Transform(vec3(0, 5, 0), vec3(0), vec3(1)), m_pResources->GetMesh("Sphere"), m_pResources->GetMaterial("Lighting2"));
	player->SetPlayerController(m_pGame->GetController(0));
	player->CreateBody(vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_pGameObjects.push_back(player);

	((ChaseCameraObject*)m_Camera)->SetObjectToFollow(player, 10.0f);

	GameObject3D* Floor = new GameObject3D(this, "Floor", Transform(vec3(0, 0, 0), vec3(0), vec3(1)), m_pResources->GetMesh("Plane"), m_pResources->GetMaterial("Lighting"));
	Floor->CreatePlane();
	//Floor->CreateBody(vec3(100.0f, 0.001f, 100.0f), 0);
	m_pGameObjects.push_back(Floor);
}

void BulletScene::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == VK_F3)
		{
			Player* player = (Player*)GetGameObjectByName("Player");
			player->GetBody()->applyTorqueImpulse(btVector3(0.0f, 1.0f, 0.0f));
		}
	}
}

void BulletScene::Update(float deltatime)
{
	Scene::Update(deltatime);
	m_BulletManager->Update(deltatime);
}

void BulletScene::Draw()
{
	Scene::Draw();
}
