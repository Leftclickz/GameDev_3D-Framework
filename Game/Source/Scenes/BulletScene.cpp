#include "GamePCH.h"
#include "BulletScene.h"

#include "Game/BulletManager.h"

#include "Game/AudioManager.h"
#include "Game/AudioDataStructures.h"
#include "Game/AudioEngine.h"
#include "Game/WeightedRandomAudioList.h"
#include "Game/ShuffleAudioList.h"

#include "GameObjects/Player.h"
#include "GameObjects/PlayerController.h"
#include "GameObjects/Enemy.h"
#include "GameObjects/ChaseCameraObject.h"
#include "GameObjects/LightObject.h"
#include "GameObjects/FollowLight.h"
#include "GameObjects/BoshyBullet.h"
#include "GameObjects/SavePoint.h"
#include "GameObjects/Objective.h"

#include "DebugDraw.h"
#include "Mesh/FBODefinition.h"

#include "HUD_Scene.h"
#include "PauseScreen.h"


BulletScene::BulletScene(Game* pGame, ResourceManager* pResources) :
	Scene(pGame, pResources),
	m_FBOobject(nullptr)
{
	m_Name = "GamePlayScreen";
	//remove the standard cam
	GameObject* cam = GetGameObjectByName("Camera");
	delete cam;
	RemoveGameObject(cam);

	//create the proper chase-cam
	m_Camera = new ChaseCameraObject(this, "ChaseCamera", Transform(vec3(0.0f, 10.0f, -15.0f)));
	AddGameObject(m_Camera);

	m_BulletManager = new BulletManager();

	m_FBO = new FBODefinition(512, 512, GL_LINEAR, GL_LINEAR, false);
	m_FBO->Invalidate(true);
}

BulletScene::~BulletScene()
{
	delete m_FBO;

	if(m_FBOobject)
		delete m_FBOobject;
}

void BulletScene::LoadContent()
{
	Scene::LoadContent();

	m_pResources->AddTexture("White", new Texture("Data/Textures/White.png"));
	m_pResources->AddTexture("Spike", new Texture("Data/Textures/Spike.png"));
	m_pResources->AddTexture("Door", new Texture("Data/Textures/Door.png"));

	m_pResources->AddMaterial("Lighting", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("Water")));
	m_pResources->AddMaterial("Lighting2", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("White")));
	m_pResources->AddMaterial("SavePoint", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("Dice")));
	m_pResources->AddMaterial("Spikes", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("Spike")));
	m_pResources->AddMaterial("Objective", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("Door")));

	//AUDIO
	{
		//Load Wav data
		m_pResources->LoadWaveData("Background Music - 1", "Background_1");
		m_pResources->LoadWaveData("Background Music - 2", "Background_2");
		m_pResources->LoadWaveData("Background Music - 3", "Background_3");
		m_pResources->LoadWaveData("Background Music - 4", "Background_4");
		m_pResources->LoadWaveData("Player Death 1", "Player_Death_1");
		m_pResources->LoadWaveData("Player Death 2", "Player_Death_2");
		m_pResources->LoadWaveData("Player Death 3", "Player_Death_3");
		m_pResources->LoadWaveData("Player Spawn 1", "Player_Spawn_1");
		m_pResources->LoadWaveData("Player Spawn 2", "Player_Spawn_2");
		m_pResources->LoadWaveData("Player Spawn 3", "Player_Spawn_3");
		m_pResources->LoadWaveData("Player Spawn 4", "Player_Spawn_4");
		m_pResources->LoadWaveData("Player Spawn 5", "Player_Spawn_5");

		//Create audio objects
		Audio* sample = m_pResources->CreateAudio("Apex and Bones", "Background Music - 1");
		m_pResources->CreateAudio("Tesla Coil", "Background Music - 2");
		m_pResources->CreateAudio("Trial of Thunder", "Background Music - 3");
		m_pResources->CreateAudio("A Shadow to Prevail", "Background Music - 4");
		m_pResources->CreateAudio("Player Spawn 1", "Player Spawn 1");
		m_pResources->CreateAudio("Player Spawn 2", "Player Spawn 2");
		m_pResources->CreateAudio("Player Spawn 3", "Player Spawn 3");
		m_pResources->CreateAudio("Player Spawn 4", "Player Spawn 4");
		m_pResources->CreateAudio("Player Spawn 5", "Player Spawn 5");
		m_pResources->CreateAudio("Player Death 1", "Player Death 1");
		m_pResources->CreateAudio("Player Death 2", "Player Death 2");
		m_pResources->CreateAudio("Player Death 3", "Player Death 3");


		//create background audio channel
		m_BackgroundChannel = m_pResources->AddAudioList("Music Soundboard", new ShuffleAudioList(sample->GetWaveFormat()));

		//set default public channels
		AudioManager::GetEngine()->SetDefaultWaveFormat(*sample->GetWaveFormat());
		AudioManager::GetEngine()->CreatePublicAudioChannels();

		//music channel population
		m_BackgroundChannel->AddAudio(m_pResources->GetAudio("Apex and Bones"));
		m_BackgroundChannel->AddAudio(m_pResources->GetAudio("Tesla Coil"));
		m_BackgroundChannel->AddAudio(m_pResources->GetAudio("Trial of Thunder"));
		m_BackgroundChannel->AddAudio(m_pResources->GetAudio("A Shadow to Prevail"));
	}

	if (m_FBO->IsFullyLoaded())
	{
		m_pResources->AddTexture("FBO", new Texture());
		m_pResources->GetTexture("FBO")->SetTectureID(m_FBO->GetColorTextureHandle());

		m_pResources->AddMaterial("FBO", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("FBO")));

		m_FBOobject = new GameObject3D(this, "TV", Transform(vec3(0), vec3(90, 0, 0)), m_pResources->GetMesh("Cube"), m_pResources->GetMaterial("FBO"));
	}

	//File Loading Time
	LoadFromSceneFile("Data/Scenes/Test3D.box2dscene");

	((ChaseCameraObject*)m_Camera)->SetObjectToFollow(GetGameObjectByName("Player"), 25.0f);
	Player * player = (Player*)GetGameObjectByName("Player");
	player->GetBody()->setAngularFactor(btVector3(0, 0, 0));

	//Scenes
	m_pSceneManager->AddScene("HUDScene", new HUD_Scene(m_pGame, m_pResources));
	m_pSceneManager->AddScene("PauseScene", new PauseScreen(m_pGame, m_pResources));
}

void BulletScene::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);
	m_Camera->OnEvent(pEvent);
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == VK_F3)
		{
			Player* player = (Player*)GetGameObjectByName("Player");
			player->GetBody()->applyTorqueImpulse(btVector3(0.0f, 1.0f, 0.0f));
		}

		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 9 && pInput->GetInputState() == InputState_Pressed)//TAB
		{
			//PAUSE Screen
			m_pGame->GetSceneManager()->PushScene("PauseScene");
			m_pGame->GetEventManager()->QueueEvent(new SceneChangeEvent(nullptr, m_pGame->GetSceneManager()->GetActiveScenes().back()));
		}

		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 'R' && pInput->GetInputState() == InputState_Pressed)//Reset
		{
			Reset();
		}
	}
}

void BulletScene::Update(float deltatime)
{
	Scene::Update(deltatime);
	m_BulletManager->Update(deltatime);
	m_BackgroundChannel->Update(deltatime);
}

void BulletScene::Draw()
{
	if (m_FBO->IsFullyLoaded())
	{
		m_FBO->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 512, 512);

		Scene::Draw();
		m_FBO->Unbind();

		
		glViewport(0, 0, m_pGame->GetFramework()->GetWindowWidth(), m_pGame->GetFramework()->GetWindowHeight());

		m_BulletManager->Draw(m_Camera, m_pResources->GetMaterial("Debug3D"));

		m_FBOobject->Draw(m_Camera);
	}
	else
	{
		Scene::Draw();

		m_BulletManager->Draw(m_Camera, m_pResources->GetMaterial("Debug3D"));
	}
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
		else if (flag == "Enemy")
		{
			Enemy* enemy = new Enemy(this, "Enemy", Transform(), nullptr, nullptr);
			enemy->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(enemy);
		}
		else if (flag == "5")
		{
			LightObject* light = new LightObject(this, "Light", Transform(), nullptr, nullptr);
			light->LoadFromcJSON(jGameObject, m_pResources);
			AddLightObject(light);
		}
		else if (flag == "6")
		{
			SavePoint* savePoint = new SavePoint(this, "SavePoint", Transform(), nullptr, nullptr);
			savePoint->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(savePoint);
		}
		else if (flag == "7")
		{
			Objective* objective = new Objective(this, "Objective", Transform(), nullptr, nullptr);
			objective->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(objective);
		}
		else if (flag == "GameObject")
		{
			GameObject3D* gameobject = new GameObject3D(this, "Object", Transform(), nullptr, nullptr);
			gameobject->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(gameobject);
		}
	}

	cJSON_Delete(jRoot);
	delete[] contents;
}

void BulletScene::HasEnteredFocus()
{
	//if (m_BackgroundChannel != nullptr)
	//{
	//	m_BackgroundChannel->PlayAudio(0);
	//}
	m_pSceneManager->PushScene("HUDScene");
}

void BulletScene::HasLeftFocus()
{
	if (m_BackgroundChannel != nullptr)
	{
		m_BackgroundChannel->StopAudio();
	}
}

void BulletScene::Reset()
{
	Scene::Reset();
}
