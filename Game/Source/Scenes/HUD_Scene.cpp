#include "GamePCH.h"
#include "HUD_Scene.h"
#include "GameObjects/HUD_TextObject.h"
#include "Mesh/TextMesh.h"
#include "Game/ResourceManager.h"
#include "GameObjects/Camera.h"
#include "GameObjects/PlatformerPlayer.h"
#include "Game/Game.h"


HUD_Scene::HUD_Scene(Game* pGame, ResourceManager* pResources)
	:Scene(pGame, pResources)
{
	m_IsTransparent = true;

	m_Name = "HudScreen";
}

HUD_Scene::~HUD_Scene()
{

}

void HUD_Scene::LoadContent()
{
	Scene::LoadContent();

	float aspect = (float)m_pGame->GetFramework()->GetWindowWidth() / (float)m_pGame->GetFramework()->GetWindowHeight();

	//score HUD object
	HUD_TextObject* ScoreDisplay = new HUD_TextObject(this, "ScoreDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	ScoreDisplay->SetString("SCORE : 0");
	ScoreDisplay->SetPosition(vec3(0.4f * aspect, 9.5f * aspect, 0.0f));
	ScoreDisplay->SetScale(vec3(0.1f, 0.1f, 0.1f));
	AddGameObject(ScoreDisplay);

	//Lives HUD object
	HUD_TextObject* LifeDisplay = new HUD_TextObject(this, "LifeDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	LifeDisplay->SetString("LIVES : 2");
	LifeDisplay->SetPosition(vec3(0.4f * aspect, 8.5f * aspect, 0.0f));
	LifeDisplay->SetScale(vec3(0.1f, 0.1f, 0.1f));
	AddGameObject(LifeDisplay);

	//VICTORY HUD object
	HUD_TextObject* VictoryDisplay = new HUD_TextObject(this, "VictoryDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	VictoryDisplay->SetString("VICTORY!");
	VictoryDisplay->SetPosition(vec3(5 * aspect, 5 * aspect, 0.0f));
	VictoryDisplay->SetScale(vec3(0.3f));
	AddGameObject(VictoryDisplay);
	VictoryDisplay->SetIsVisible(false);

	//DEFEAT HUD object
	HUD_TextObject* DefeatDisplay = new HUD_TextObject(this, "DefeatDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	DefeatDisplay->SetString("LUL YOU DIED!");
	DefeatDisplay->SetPosition(vec3(5 * aspect, 5 * aspect, 0.0f));
	DefeatDisplay->SetScale(vec3(0.2f));
	AddGameObject(DefeatDisplay);
	DefeatDisplay->SetIsVisible(false);

	//camera setup
	m_Camera->SetIsOrtho(true);
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetPosition(vec3(0.0f, 0.0f, -5.0f));
}

void HUD_Scene::OnEvent(Event* pEvent)
{
	//Scene::OnEvent(pEvent);

	EventTypes type = pEvent->GetEventType();

	//If we're doing a value adjustment get the type and apply the new string
	if (type == EventType_HUDValueAdjust)
	{
		HUD_ValueAdjustEvent* ev = (HUD_ValueAdjustEvent*) pEvent;

		ValueToAdjust adjustType = ev->GetAdjustType();
		HUD_TextObject* HUD;

		switch (adjustType)
		{
		case Value_Score:
			HUD = (HUD_TextObject*)GetGameObjectByName("ScoreDisplay");
			HUD->SetString("SCORE : " + std::to_string(ev->GetValue()));
			break;
		case Value_Life:
			HUD = (HUD_TextObject*)GetGameObjectByName("LifeDisplay");
			HUD->SetString("LIVES : " + std::to_string(ev->GetValue()));
			break;
		default:
			break;
			
		}
	}
	//Game state event handling
	else if (type == EventType_GameState)
	{
		GameStateEvent* ev = (GameStateEvent*)pEvent;
		HUD_TextObject* HUD;

		//Turns various text boxes off/on
		switch (ev->GetGameState())
		{
		case PlayState:
			HUD = (HUD_TextObject*)GetGameObjectByName("DefeatDisplay");
			HUD->SetIsVisible(false);
			HUD = (HUD_TextObject*)GetGameObjectByName("VictoryDisplay");
			HUD->SetIsVisible(false);
			m_DoesPause = false;
			break;
		case WinState:
			HUD = (HUD_TextObject*)GetGameObjectByName("VictoryDisplay");
			HUD->SetIsVisible(true);
			m_DoesPause = true;
			break;
		case LoseState:
			HUD = (HUD_TextObject*)GetGameObjectByName("DefeatDisplay");
			HUD->SetIsVisible(true);
			m_DoesPause = true;
			break;
		default:
			break;
			
		}
	}
}

void HUD_Scene::Update(float deltatime)
{
	Scene::Update(deltatime);
}

void HUD_Scene::Draw()
{
	Scene::Draw();
}

void HUD_Scene::SetTextBoxString(std::string text_box_name, std::string text)
{
	((HUD_TextObject*)GetGameObjectByName(text_box_name))->SetString(text);
}

void HUD_Scene::Reset()
{
	Scene::Reset();

	((HUD_TextObject*)GetGameObjectByName("DefeatDisplay"))->SetIsVisible(false);
	((HUD_TextObject*)GetGameObjectByName("VictoryDisplay"))->SetIsVisible(false);

	m_DoesPause = false;
}

