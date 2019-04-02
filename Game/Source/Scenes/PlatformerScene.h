#pragma once
#include "Scenes/Scene.h"
#include "GameObjects/Bullet.h"

class DebugDraw;
class Audio;
class SharedAudioChannel;

class PlatformerScene : public Scene
{
public:
	PlatformerScene(Game* pGame, ResourceManager* pResources);
	virtual ~PlatformerScene();

	virtual void LoadContent() override;
	virtual void OnEvent(Event* pEvent) override;
	virtual void Update(float deltatime) override;
	virtual void Draw() override;

	void Victory();
	void GameOver();

	virtual void Reset() override;

	//Set lerping for the main camera to bind to a target (offset optional)
	void StartLerp(GameObject* obj, vec3 offset = vec3(0));

	//Lerp our camera to our game object using a time coefficient
	void Lerp(Camera* cam, GameObject* obj, float deltatime);

	//set flags for next frame to do a clean Reset state
	void PreReset();

	virtual void LoadFromSceneFile(std::string filename) override;

	virtual void HasEnteredFocus() override;
	virtual void HasLeftFocus() override;

	Pool<GameObject*> m_BulletPool;
protected:
	float m_GameOverTimer = 3.0f;

	enum EGameState m_GameState;


	//Vars to contain LERPing
	GameObject* m_LERPTarget = nullptr;
	vec3 m_LERPOffset = vec3(0);

	Audio* m_GameAudio;
	class AudioList* m_BackgroundChannel;

	float m_Timer = 0.0f;
};