#pragma once
#include "Scenes/Scene.h"

class DebugDraw;
class BulletManager;
class ChaseCameraObject;

class BulletScene : public Scene
{
public:
	BulletScene(Game* pGame, ResourceManager* pResources);
	virtual ~BulletScene();

	virtual void LoadContent() override;
	virtual void OnEvent(Event* pEvent) override;
	virtual void Update(float deltatime) override;
	virtual void Draw() override;

	virtual void LoadFromSceneFile(std::string filename) override;
};