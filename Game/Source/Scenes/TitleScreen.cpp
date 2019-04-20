#include "GamePCH.h"
#include "TitleScreen.h"

#include "GameObjects/HUD_TextObject.h"
#include "Mesh/TextMesh.h"

#include "BulletScene.h"
#include "OptionsScene.h"

TitleScreen::TitleScreen(Game* pGame, ResourceManager* pResources)
	:Scene(pGame, pResources)
{
	m_IsTransparent = true;
	m_DoesPause = true;

	m_Name = "TitleScreen";
	m_Timer = 3.0f;
	m_isGrowing = true;
}

TitleScreen::~TitleScreen()
{

}

void TitleScreen::LoadContent()
{
	Scene::LoadContent();

	m_pSceneManager->PushScene("TitleScene");

	m_pSceneManager->AddScene("BulletScene", new BulletScene(m_pGame, m_pResources));
	m_pSceneManager->AddScene("OptionsScene", new OptionsScene(m_pGame, m_pResources));

	//camera setup
	m_Camera->SetIsOrtho(true);
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetPosition(vec3(0.0f, 0.0f, -5.0f));

	//DEFEAT HUD object
	HUD_TextObject* TitleDisplay = new HUD_TextObject(this, "TitleDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	TitleDisplay->SetString("PRESS ENTER TO START");
	TitleDisplay->SetPosition(vec3(1.0f, 7.0f, 0.0f));
	TitleDisplay->SetScale(vec3(0.25f));
	AddGameObject(TitleDisplay);

	HUD_TextObject* OptionsDisplay = new HUD_TextObject(this, "OptionsDisplay", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	OptionsDisplay->SetString("PRESS O FOR OPTIONS");
	OptionsDisplay->SetPosition(vec3(3.0f, 4.0f, 0.0f));
	OptionsDisplay->SetScale(vec3(0.2f));
	AddGameObject(OptionsDisplay);
}

void TitleScreen::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;

		//if they press enter remove this scene and pop on the gameplay scene
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 13)//ENTER
		{
			m_pSceneManager->Pop();

			m_pSceneManager->PushScene("BulletScene");
		}

		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 'O')
		{
			m_pSceneManager->PushScene("OptionsScene");
		}
	}
}

void TitleScreen::Update(float deltatime)
{
	Scene::Update(deltatime);

	m_Timer += deltatime;

	if (m_Timer >= 3.0f)
	{
		m_Timer -= 3.0f;

		if (m_isGrowing)
		{
			GetGameObjectByName("TitleDisplay")->AddTween(TweenType_Scale, vec3(-0.1f), 3.0f, TweenFunc_Linear);
			m_isGrowing = false;
		}
		else
		{
			GetGameObjectByName("TitleDisplay")->AddTween(TweenType_Scale, vec3(0.1f), 3.0f, TweenFunc_Linear);
			m_isGrowing = true;
		}

	}

}

void TitleScreen::Draw()
{
	Scene::Draw();
}

void TitleScreen::SetTextBoxString(std::string text_box_name, std::string text)
{
	((HUD_TextObject*)GetGameObjectByName(text_box_name))->SetString(text);
}

void TitleScreen::Reset()
{
	Scene::Reset();
}