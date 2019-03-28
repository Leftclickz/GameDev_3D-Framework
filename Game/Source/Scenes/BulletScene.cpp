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
#include "GameObjects/LightObject.h"
#include "GameObjects/FollowLight.h"

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

	//LoadFromSceneFile("Data/Scenes/Test3D.box2dscene");

	Player* player = new Player(this, "Player", Transform(vec3(0, 5, 0), vec3(0), vec3(1)), m_pResources->GetMesh("Sphere"), m_pResources->GetMaterial("Lighting2"));
	player->SetPlayerController(m_pGame->GetController(0));
	//player->CreateBoxBody(vec3(0.5f, 0.5f, 0.5f), 1.0f);
	player->CreateSphereBody(1.0f, 1.0f);
	//player->CreateConvexHullBody(1.0f);

	((ChaseCameraObject*)m_Camera)->SetObjectToFollow(player, 10.0f);

	GameObject3D* Floor = new GameObject3D(this, "Floor", Transform(vec3(0, 0, 0), vec3(0), vec3(1)), m_pResources->GetMesh("Plane"), m_pResources->GetMaterial("Lighting"));
	Floor->CreatePlane();
	//Floor->GetBody()->setFriction(100);

	for (int x = 0; x < 10; x++)
	{
		GameObject3D* box = new GameObject3D(this, "Box" + std::to_string(x), Transform(vec3((float)x * 2.0f, 4, 4), vec3(0), vec3(1)), m_pResources->GetMesh("Cube"), m_pResources->GetMaterial("Lighting2"));
		box->CreateBoxBody(vec3(0.5f, 0.5f, 0.5f), 5);
	}

	LightObject* light = new LightObject(this, "Red", Transform(vec3(-2, 8, 0), vec3(0), vec3(1)), nullptr, nullptr);
	light->AssignLightColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pLights.push_back(light);

	light = new LightObject(this, "Green", Transform(vec3(2, 8, 0), vec3(0), vec3(1)), nullptr, nullptr);
	light->AssignLightColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	m_pLights.push_back(light);

	light = new LightObject(this, "Blue", Transform(vec3(0, 8, 0), vec3(0), vec3(1)), nullptr, nullptr);
	light->AssignLightColor(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	m_pLights.push_back(light);

	FollowLight* pLight = new FollowLight(this, "PlayerLight", Transform(vec3(0, 8, 0), vec3(0), vec3(1)), nullptr, nullptr);
	pLight->SetObjectAttachment(player);
	pLight->SetFollowOffset(vec3(0, 2, 0));
	pLight->SetAttenuationFactor(0.15f);
	pLight->AssignLightColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pLights.push_back(pLight);
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
	m_BulletManager->Draw(m_Camera ,m_pResources->GetMaterial("Debug3D"));
}

void BulletScene::LoadFromSceneFile(std::string filename)
{
	char* contents = LoadCompleteFile(filename.c_str(), nullptr);

	cJSON* jRoot = cJSON_Parse(contents);
	cJSON* jGameObjectArray = cJSON_GetObjectItem(jRoot, "GameObjects");

	int numobjects = cJSON_GetArraySize(jGameObjectArray);

	for (int i = 0; i < numobjects; i++)
	{
		cJSON* jGameObject = cJSON_GetArrayItem(jGameObjectArray, i);


		cJSON* jFlagArray = cJSON_GetObjectItem(jGameObject, "Flags");

		//only get the first flag.
		std::string flag;
		if (cJSON_GetArrayItem(jFlagArray, 0) != nullptr)
		{
			flag = cJSON_GetArrayItem(jFlagArray, 0)->valuestring;
		}
		else
		{
			flag = "GameObject";
		}

		//player
		if (flag == "Player")
		{
			Player* player = new Player(this, "Player", Transform(), nullptr, nullptr);
			player->SetPlayerController(m_pGame->GetController(0));
			player->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(player);
		}
		else if (flag == "GameObject")
		{
			GameObject3D* gameobject = new GameObject3D(this, "Object", Transform(), m_pResources->GetMesh("Box"), nullptr);
			gameobject->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(gameobject);
		}
	}

	cJSON_Delete(jRoot);
	delete[] contents;
}
