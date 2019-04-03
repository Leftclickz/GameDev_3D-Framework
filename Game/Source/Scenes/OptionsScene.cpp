#include "GamePCH.h"
#include "OptionsScene.h"

#include "GameObjects/HUD_TextObject.h"
#include "Mesh/TextMesh.h"
#include "BulletScene.h"


OptionsScene::OptionsScene(Game* pGame, ResourceManager* pResources)
	:Scene(pGame, pResources)
{
	m_IsTransparent = false;
	m_DoesPause = true;
	m_DoesStopEvents = true;

	m_Name = "OptionsScreen";
}

OptionsScene::~OptionsScene()
{

}

void OptionsScene::LoadContent()
{
	Scene::LoadContent();

	//camera setup
	m_Camera->SetIsOrtho(true);
	m_Camera->SetLookAtPosition(vec3(0.0f, 0.0f, 0.0f));
	m_Camera->SetPosition(vec3(0.0f, 0.0f, -5.0f));

	//DEFEAT HUD object
	HUD_TextObject* OptionOne = new HUD_TextObject(this, "OptionOne", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	OptionOne->SetString("OPTION 1");
	OptionOne->SetPosition(vec3(1.0f, 9.0f, 0.0f));
	OptionOne->SetScale(vec3(0.1f));
	AddGameObject(OptionOne);

	HUD_TextObject* OptionTwo = new HUD_TextObject(this, "OptionTwo", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	OptionTwo->SetString("OPTION 2");
	OptionTwo->SetPosition(vec3(1.0f, 7.0f, 0.0f));
	OptionTwo->SetScale(vec3(0.1f));
	AddGameObject(OptionTwo);

	HUD_TextObject* Back = new HUD_TextObject(this, "Back", Transform(), new TextMesh(), m_pResources->GetMaterial("Text"));
	Back->SetString("PRESS TAB TO GO BACK");
	Back->SetPosition(vec3(7.0f, 2.0f, 0.0f));
	Back->SetScale(vec3(0.1f));
	AddGameObject(Back);
}

void OptionsScene::OnEvent(Event* pEvent)
{
	Scene::OnEvent(pEvent);
	if (pEvent->GetEventType() == EventType_Input)
	{
		InputEvent* pInput = (InputEvent*)pEvent;

		//if they press enter remove this scene and pop on the gameplay scene
		if (pInput->GetInputDeviceType() == InputDeviceType_Keyboard && pInput->GetID() == 9)//Tab
		{
			m_pSceneManager->Pop();
		}
	}
}

void OptionsScene::Update(float deltatime)
{
	Scene::Update(deltatime);
}

void OptionsScene::Draw()
{
	Scene::Draw();
}

void OptionsScene::SetTextBoxString(std::string text_box_name, std::string text)
{
	((HUD_TextObject*)GetGameObjectByName(text_box_name))->SetString(text);
}

void OptionsScene::Reset()
{
	Scene::Reset();
}