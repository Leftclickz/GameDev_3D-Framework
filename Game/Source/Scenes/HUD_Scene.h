#pragma once
#include "Scene.h"

class HUD_Scene : public Scene
{
public:

	HUD_Scene(Game* pGame, ResourceManager* pResources);
	virtual ~HUD_Scene();

	virtual void LoadContent() override;

	//Primary use cases here are adjusting text boxes and enabling visibility
	virtual void OnEvent(Event* pEvent) override;

	virtual void Update(float deltatime) override;

	virtual void Draw() override;

	//Fetches a text box and sets the string inside of it
	virtual void SetTextBoxString(std::string text_box_name, std::string text);

	virtual void Reset() override;

protected:

};