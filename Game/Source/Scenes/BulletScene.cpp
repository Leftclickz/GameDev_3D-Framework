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
#include "GameObjects/ChaseCameraObject.h"
#include "GameObjects/LightObject.h"
#include "GameObjects/FollowLight.h"

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

	delete m_BackgroundChannel;
}

void BulletScene::LoadContent()
{
	Scene::LoadContent();

	m_pResources->AddTexture("White", new Texture("Data/Textures/White.png"));

	m_pResources->AddMaterial("Lighting", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("Water")));
	m_pResources->AddMaterial("Lighting2", new Material(m_pResources->GetShader("LightingShader"), m_pResources->GetTexture("White")));

	//AUDIO
	m_pResources->LoadWaveData("Main Music", "Floor_1");
	m_pResources->LoadWaveData("Player Swing 1", "Player_Swing_1");
	m_pResources->LoadWaveData("Player Swing 2", "Player_Swing_2");
	m_pResources->LoadWaveData("Player Swing 3", "Player_Swing_3");
	m_pResources->LoadWaveData("Player Swing 4", "Player_Swing_4");

	//create audio voice
	m_GameAudio = m_pResources->CreateAudio("Main Music", "Main Music");

	m_BackgroundChannel = new WeightedRandomAudioList();

	AudioManager::GetEngine()->SetDefaultWaveFormat(*m_GameAudio->GetWaveFormat());
	AudioManager::GetEngine()->CreatePublicAudioChannels();

	m_BackgroundChannel->AddAudio(m_pResources->CreateAudio("Player Swing 1", "Player Swing 1"));
	m_BackgroundChannel->AddAudio(m_pResources->CreateAudio("Player Swing 2", "Player Swing 2"));
	m_BackgroundChannel->AddAudio(m_pResources->CreateAudio("Player Swing 3", "Player Swing 3"));
	m_BackgroundChannel->AddAudio(m_pResources->CreateAudio("Player Swing 4", "Player Swing 4"));
	m_BackgroundChannel->SetName("Soundboard");

	if (m_FBO->IsFullyLoaded())
	{
		m_pResources->AddTexture("FBO", new Texture());
		m_pResources->GetTexture("FBO")->SetTectureID(m_FBO->GetColorTextureHandle());

		m_pResources->AddMaterial("FBO", new Material(m_pResources->GetShader("TextureShader"), m_pResources->GetTexture("FBO")));

		m_FBOobject = new GameObject3D(this, "TV", Transform(vec3(0), vec3(90, 0, 0)), m_pResources->GetMesh("ObjCube"), m_pResources->GetMaterial("FBO"));
	}

	//File Loading Time
	LoadFromSceneFile("Data/Scenes/Test3D.box2dscene");

	((ChaseCameraObject*)m_Camera)->SetObjectToFollow(GetGameObjectByName("Player"), 10.0f);
	Player * player = (Player*)GetGameObjectByName("Player");
	player->GetBody()->setAngularFactor(btVector3(0, 0, 0));

	//Scenes
	m_pSceneManager->AddScene("HUDScene", new HUD_Scene(m_pGame, m_pResources));
	m_pSceneManager->AddScene("PauseScene", new PauseScreen(m_pGame, m_pResources));
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

		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 9 && pInput->GetInputState() == InputState_Pressed)//TAB
		{
			//PAUSE Screen
			m_pGame->GetSceneManager()->PushScene("PauseScene");
			m_pGame->GetEventManager()->QueueEvent(new SceneChangeEvent(nullptr, m_pGame->GetSceneManager()->GetActiveScenes().back()));
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
		else if (flag == "GameObject")
		{
			GameObject3D* gameobject = new GameObject3D(this, "Object", Transform(), nullptr, nullptr);
			gameobject->LoadFromcJSON(jGameObject, m_pResources);
			AddGameObject(gameobject);
		}
		else if (flag == "5")
		{
			LightObject* light = new LightObject(this, "Light", Transform(), nullptr, nullptr);
			light->LoadFromcJSON(jGameObject, m_pResources);
			AddLightObject(light);
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
