#include "GamePCH.h"
#include "PlatformerScene.h"

#include "../Game/Game.h"
#include "Game/ResourceManager.h"
#include "Scene.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Player.h"
#include "GameObjects/PlayerController.h"
#include "../Physics/PhysicsWorld.h"
#include "Mesh/SpriteSheet.h"
#include "GameObjects/Camera.h"
#include "Game/Pool.h"

#include "DebugDraw.h"
#include <string>
#include "GameObjects/PlatformerPlayer.h"
#include "GameObjects/HUD_TextObject.h"
#include "Mesh/TextMesh.h"
#include "HUD_Scene.h"
#include "GameObjects/GameObject2D.h"
#include "GameObjects/Shootable.h"
#include "GameObjects/PlatformerEnemy.h"
#include "Game/AudioDataStructures.h"

PlatformerScene::PlatformerScene(Game* pGame, ResourceManager* pResources) :
	Scene(pGame, pResources)
{
	m_Camera->SetPosition(vec3(0, 0, -25));
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetRange(200.0f);

	m_GameAudio = nullptr;

	m_GameState = EGameState::PlayState;
}

PlatformerScene::~PlatformerScene()
{
	Reset();
}

void PlatformerScene::LoadContent()
{
	//create the textures we'll need for boshy later
	m_pResources->AddTexture("BloodyBoshy", new Texture("Data/Textures/BloodyBoshy.png"));
	m_pResources->AddTexture("VictoryBoshy", new Texture("Data/Textures/VictoryBoshy.png"));
	m_pResources->AddTexture("Ground", new Texture("Data/Textures/Ground.png"));
	m_pResources->AddTexture("Spike", new Texture("Data/Textures/Spike.png"));
	m_pResources->AddTexture("Boshy", new Texture("Data/Textures/Boshy.png"));
	m_pResources->AddTexture("Coin", new Texture("Data/Textures/Coin.png"));
	m_pResources->AddTexture("Red", new Texture("Data/Textures/Red.png"));
	m_pResources->AddTexture("Bullet", new Texture("Data/Textures/Bullet.png"));
	m_pResources->AddTexture("Door", new Texture("Data/Textures/Door.png"));
	m_pResources->AddTexture("Elevator", new Texture("Data/Textures/Elevator.png"));

	m_pResources->AddMaterial("Red", new Material(m_pResources->GetShader("NormalShader"), m_pResources->GetTexture("Red")));
	m_pResources->AddMaterial("BloodyBoshy", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("BloodyBoshy")));
	m_pResources->AddMaterial("VictoryBoshy", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("VictoryBoshy")));
	m_pResources->AddMaterial("Meteor", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Meteor")));
	m_pResources->AddMaterial("Spike", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Spike")));
	m_pResources->AddMaterial("Boshy", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Boshy")));
	m_pResources->AddMaterial("Coin", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Coin")));
	m_pResources->AddMaterial("Ground", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Ground")));
	m_pResources->AddMaterial("Bullet", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Bullet")));
	m_pResources->AddMaterial("Door", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Door")));
	m_pResources->AddMaterial("Elevator", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("Elevator")));
	
	m_ShouldFilter = true;

	Scene::LoadContent();

	//load wave file
	m_pResources->LoadWaveData("Main Music", "Floor_1");
	
	//load our scene
	LoadFromSceneFile("Data/Scenes/Platforming.box2dscene");


	//create audio voice
	m_GameAudio = m_pResources->CreateAudio("Main Music", "Main Music");


	//Collision Filtering Setup time
	b2Filter PickupFilter;
	PickupFilter.categoryBits = Collision_Pickup;
	PickupFilter.maskBits = Collision_Player;

	//loop through every coin object and give it a filter since they are created as standard GameObjects
	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
	{
		GameObject2D* pObj = (GameObject2D*)m_pGameObjects[i];

		if (pObj->GetName() == "Coin")
		{
			pObj->GetBody()->GetFixtureList()->SetFilterData(PickupFilter);
		}
	}

	//create our 1 object with a 3dsmax exported mesh
	GameObject* specialMesh = GetGameObjectByName("SpecialMesh");
 	if (specialMesh)
 	{
		specialMesh->SetMesh(m_pResources->GetMesh("Teapot"));
 	}

	//Bullets aren't from cJSON and need a pool
	b2Filter BulletFilter;
	BulletFilter.categoryBits = Collision_Bullet;
	BulletFilter.maskBits = Collision_Environment | Collision_Enemy | Collision_Shootable;
	for (unsigned int i = 0; i < 5; i++)
	{
		Bullet* b = new Bullet(this, "Bullet", Transform(vec3(0.0f, -500.0f, 0.0f), vec3(0), vec3(1)), m_pResources->GetMesh("Bullet"), m_pResources->GetMaterial("Bullet"));
		b->CreateBody(false);
		b->AddCircle(0.25f, b2Vec2(0.0f, 0.0f), 1.0f, false, 0.4f, 0.0f);
		b->GetBody()->SetActive(true);
		b->GetBody()->GetFixtureList()->SetFilterData(BulletFilter);
		b->GetBody()->SetFixedRotation(true);
		b->SetPool(&m_BulletPool);

		m_BulletPool.AddObjectToPool(b);
	}

	//make the audio loop and play
	if (m_GameAudio != nullptr)
	{
		m_GameAudio->SetDoesLoop(true);
		m_GameAudio->SetVolume(0.1f);
		//m_GameAudio->Play();
		//m_GameAudio->Pause();
	}
}

void PlatformerScene::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);

	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;

		//Hard Reset
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 'R')
		{
			PreReset();
		}
	}

	//event to reset the body's to off the map (bullets and destructable things)
	if (pEvent->GetEventType() == EventType_TranslateObject)
	{
		TranslateObjectEvent* TE = (TranslateObjectEvent*)pEvent;

		GameObject2D* pObj = (GameObject2D*)TE->GetTranslatingObject();

		if (pObj)
		{
			float x = 0;
			float y = 0;
			float z = 0;

			TE->GetPosition(x, y, z);
			pObj->GetBody()->SetTransform(b2Vec2(x, y), 0);
			pObj->GetBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}
	}
}

void PlatformerScene::Update(float deltatime)
{
	//our debugs
	ImGui::Checkbox("Wireframe", &m_Wireframe);
	ImGui::Checkbox("Debug", &m_DebugDraw);

	Scene::Update(deltatime);

	//if we won or lost start the timer
	if (m_GameState == WinState || m_GameState == LoseState)
	{
		m_GameOverTimer -= deltatime;

		//Lerp while it's still going on
		if (m_LERPTarget != nullptr)
		{
			Lerp(m_Camera, m_LERPTarget, deltatime);
		}

		//when the timer ends reset everything
		if (m_GameOverTimer <= 0)
		{
			PreReset();
		}
	}
}

void PlatformerScene::Draw()
{
	Scene::Draw();
}

//called when the player reaches the goal
void PlatformerScene::Victory()
{
	//stop input
	PlatformerPlayer* player = (PlatformerPlayer*)GetGameObjectByName("PlatformerPlayer");
	if (player)
	{
		//make boshy immovable and use the victory material
		m_GameState = WinState;
		player->m_GameState = m_GameState;
		player->SetMaterial(m_pResources->GetMaterial("VictoryBoshy"));

		//pass a victory event for the hud
		m_pGame->GetEventManager()->QueueEvent(new GameStateEvent(m_GameState));

		//Do the camera shit and reset here
		//m_Camera->LERPToTarget(2.0f, player, vec3(0, 0, -10));
		StartLerp(player, vec3(0, 0, -18));
	}
}

//called when the player dies with no more lives
void PlatformerScene::GameOver()
{
	//stop input
	PlatformerPlayer* player = (PlatformerPlayer*)GetGameObjectByName("PlatformerPlayer");

	if (player)
	{
		//make boshy immovable and use the defeat material
		m_GameState = LoseState;
		player->m_GameState = m_GameState;
		player->SetMaterial(m_pResources->GetMaterial("BloodyBoshy"));
		
		//pass a defeat event for the hud
		m_pGame->GetEventManager()->QueueEvent(new GameStateEvent(m_GameState));

		//Do the camera shit and reset here
		//m_Camera->LERPToTarget(2.0f, player, vec3(0, 0, -10));
		StartLerp(player, vec3(0, 0, -18));
	}
}

//resets the entire scene and every object in it
void PlatformerScene::Reset()
{
	//reset cam
	m_Camera->ClearLookAtTarget();
	m_Camera->SetPosition(vec3(0, 0, -25));
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetRange(200.0f);

	//remove any bullets from the world
	for (unsigned int i = 0; i < m_pGameObjects.size(); i++)
	{
 		if (m_pGameObjects[i]->GetName() == "Bullet")
 		{
			Bullet* bullet = (Bullet*)m_pGameObjects[i];
			bullet->RemoveSelf();
			i--;
 		}

		//set all coins to have the right collision just in case
		if (m_pGameObjects[i]->GetName() == "Coin")
		{
			b2Filter filt;
			filt.categoryBits = Collision_Pickup;
			filt.maskBits = ((GameObject2D*)m_pGameObjects[i])->GetBody()->GetFixtureList()->GetFilterData().maskBits;

			((GameObject2D*)m_pGameObjects[i])->GetBody()->GetFixtureList()->SetFilterData(filt);
		}
	}

	//must be done last
	Scene::Reset();
}

void PlatformerScene::StartLerp(GameObject* obj, vec3 offset)
{
	m_LERPTarget = obj;
	m_LERPOffset = offset;
}

void PlatformerScene::Lerp(Camera* cam, GameObject* obj, float deltatime)
{
	//percent of delta on timer
	float deltaPercent = deltatime / 3.0f;

	//delta values of our lookat, position and offset
	vec3 LookAtDelta = obj->GetPosition() * deltaPercent;
	vec3 PositionDelta = obj->GetPosition() * deltaPercent;
	vec3 OffsetDelta = (cam->GetOffset() - m_LERPOffset) * deltaPercent;

	//add the deltas
	cam->SetLookAtPosition(cam->GetLookAtPosition() + LookAtDelta);
	cam->SetPosition(cam->GetPosition() + PositionDelta + OffsetDelta);
}

void PlatformerScene::PreReset()
{
	//Set sensitive vars back to defaults.
	m_GameOverTimer = 3.0f;
	m_LERPTarget = nullptr;
	m_LERPOffset = vec3(0);

	//pass a playstate event
	m_GameState = PlayState;
	m_pGame->GetEventManager()->QueueEvent(new GameStateEvent(m_GameState));

	//queue the reset for next frame
	PlatformerPlayer* player = (PlatformerPlayer*)GetGameObjectByName("PlatformerPlayer");
	if (player)
	{
		player->m_GameState = m_GameState;
		player->hardReset = true;
		player->SetMaterial(m_pResources->GetMaterial("Boshy"));

		m_ShouldReset = true;
	}
}

void PlatformerScene::LoadFromSceneFile(std::string filename)
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
		if (flag == "5" || flag == "Player")
		{
			PlatformerPlayer* platformerPlayer = new PlatformerPlayer(this, "PlatformerPlayer", Transform(), nullptr, nullptr);
			platformerPlayer->SetPlayerController(m_pGame->GetController(0));
			platformerPlayer->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(platformerPlayer);

			if (m_ShouldFilter)
			{
				b2Filter PlayerFilter;
				PlayerFilter.categoryBits = Collision_Player;
				PlayerFilter.maskBits = Collision_Environment | Collision_Enemy | Collision_Pickup | Collision_Shootable;

				platformerPlayer->GetBody()->GetFixtureList()->SetFilterData(PlayerFilter);
				platformerPlayer->GetBody()->SetFixedRotation(true);

			}
		}
		else if (flag == "Enemy")
		{
			PlatformerEnemy* platformerEnemy = new PlatformerEnemy(this, "Enemy", Transform(), nullptr, nullptr);
			platformerEnemy->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(platformerEnemy);

			if (m_ShouldFilter)
			{
				b2Filter EnemyFilter;
				EnemyFilter.categoryBits = Collision_Enemy;
				EnemyFilter.maskBits = Collision_Environment | Collision_Player | Collision_Bullet;

				platformerEnemy->GetBody()->GetFixtureList()->SetFilterData(EnemyFilter);
			}
		}
		else if (flag == "Target")
		{
			Shootable* shootable = new Shootable(this, "Shootable", Transform(), nullptr, nullptr);
			shootable->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(shootable);

			if (m_ShouldFilter)
			{
				b2Filter shootableFilter;
				shootableFilter.categoryBits = Collision_Shootable;
				shootableFilter.maskBits = Collision_Environment | Collision_Player | Collision_Bullet;

				shootable->GetBody()->GetFixtureList()->SetFilterData(shootableFilter);
			}
		}
		else if (flag == "GameObject")
		{
			GameObject2D* gameobject = new GameObject2D(this, "Object", Transform(), m_pResources->GetMesh("Box"), nullptr);
			gameobject->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(gameobject);

			if (m_ShouldFilter)
			{
				b2Filter WorldFilter;
				WorldFilter.categoryBits = Collision_Environment;
				WorldFilter.maskBits = Collision_Environment | Collision_Enemy | Collision_Player | Collision_Bullet | Collision_Shootable;

				gameobject->GetBody()->GetFixtureList()->SetFilterData(WorldFilter);

			}
		}
	}

	//add another loop to handlejoints on every object
	char* name = new char[50];
	for (int i = 0; i < numobjects; i++)
	{
		cJSON* jGameObject = cJSON_GetArrayItem(jGameObjectArray, i);

		cJSONExt_GetString(jGameObject, "Name", name, 50);

		GameObject2D* pObj = (GameObject2D*)GetGameObjectByName(name);

		if (pObj)
		{
			pObj->LoadJointsFromcJSON(jGameObject);
		}
	}
	delete[] name;

	cJSON_Delete(jRoot);
	delete[] contents;
}

void PlatformerScene::HasEnteredFocus()
{
	m_GameAudio->Play();
}

void PlatformerScene::HasLeftFocus()
{
	m_GameAudio->Stop();
}
