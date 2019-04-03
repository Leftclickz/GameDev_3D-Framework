#include "GamePCH.h"
#include "PauseScreen.h"
#include "GameObjects/HUD_TextObject.h"
#include "Mesh/TextMesh.h"
#include "GameObjects/PlatformerPlayer.h"


PauseScreen::PauseScreen(Game* pGame, ResourceManager* pResources)
	:Scene(pGame, pResources)
{
	m_IsTransparent = true;
	m_DoesPause = true;

	m_Name = "PauseScreen";
}

PauseScreen::~PauseScreen()
{

}

void PauseScreen::LoadContent()
{
	Scene::LoadContent();

	//DEFEAT HUD object
	HUD_TextObject* PauseDisplay = new HUD_TextObject(this, "PauseDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	PauseDisplay->SetString("PAUSE");
	PauseDisplay->SetPosition(vec3(1.0f, 7.0f, 0.0f));
	AddGameObject(PauseDisplay);

	//camera setup
	m_Camera->SetIsOrtho(true);
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetPosition(vec3(0.0f, 0.0f, -5.0f));
}

void PauseScreen::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;

		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 'H')//TAB
		{
			m_pSceneManager->Pop();
		}
	}
}

void PauseScreen::Update(float deltatime)
{
	Scene::Update(deltatime);
}

void PauseScreen::Draw()
{
	Scene::Draw();
}

void PauseScreen::SetTextBoxString(std::string text_box_name, std::string text)
{
	((HUD_TextObject*)GetGameObjectByName(text_box_name))->SetString(text);
}

void PauseScreen::Reset()
{
	Scene::Reset();
	m_pSceneManager->Pop();
}

