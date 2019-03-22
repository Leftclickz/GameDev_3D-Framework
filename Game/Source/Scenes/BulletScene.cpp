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
#include "Game/BulletManager.h"

BulletScene::BulletScene(Game* pGame, ResourceManager* pResources) :
	Scene(pGame, pResources)
{
	m_Camera->SetPosition(vec3(0.0f, 5.0f, -15.0f));
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));

	m_BulletManager = new BulletManager();
}

BulletScene::~BulletScene()
{
}

void BulletScene::LoadContent()
{
	Scene::LoadContent();

	Player* player = new Player(this, "Player", Transform(vec3(0, 5, 0), vec3(0), vec3(1)), m_pResources->GetMesh("Cube"), m_pResources->GetMaterial("Lighting2"));
	player->SetPlayerController(m_pGame->GetController(0));
	player->CreateBody(vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_pGameObjects.push_back(player);

	GameObject3D* Floor = new GameObject3D(this, "Floor", Transform(vec3(0, 0, 0), vec3(0), vec3(1)), m_pResources->GetMesh("Plane"), m_pResources->GetMaterial("Lighting"));
	Floor->CreatePlane();
	m_pGameObjects.push_back(Floor);
}

void BulletScene::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);
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
