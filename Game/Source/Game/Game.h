#ifndef __Game_H__
#define __Game_H__

class Texture;
class Mesh;
class GameObject;
class Player;
class PlayerController;
class ResourceManager;
class Scene;
class ImGuiManager;
class HUD_Scene;
class SceneManager;

class Game : public GameCore
{
protected:
    PlayerController* m_pControllers[4];

	ResourceManager* m_pResourceManager;
	ImGuiManager* m_pImGuiManager;
	SceneManager* m_pSceneManager;

public:
    Game(Framework* pFramework);
    virtual ~Game();

    virtual void OnSurfaceChanged(unsigned int width, unsigned int height) override;
    virtual void LoadContent() override;

    virtual void OnEvent(Event* pEvent) override;
    virtual void Update(float deltatime) override;
    virtual void Draw() override;

	virtual void Reset();

    PlayerController* GetController(int index) { return m_pControllers[index]; }

	SceneManager* GetSceneManager() { return m_pSceneManager; }
};

#endif //__Game_H__
