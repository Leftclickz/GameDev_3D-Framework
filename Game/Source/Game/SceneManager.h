#pragma once

class Scene;

class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

	virtual void Update(float delta);
	virtual void Draw();

	//tries to push the given scene
	//if it cannot find the passed name it returns false
	//on success returns true
	virtual bool PushScene(std::string sceneName);
	virtual void Pop();

	virtual void PopAllScenes();

	//adds a scene so it can be pushed onto the stack.
	//calls its loadcontent
	//A scene not first added CANNOT be pushed.
	//returns true if successful
	virtual bool AddScene(std::string name, Scene* pScene);
	virtual bool IsSceneAdded(std::string name);
	virtual Scene* GetSceneByName(std::string name);

	virtual void OnEvent(Event* pEvent);

	virtual void Reset();

	const std::vector<Scene*> GetActiveScenes() { return m_ActiveScenes; }

protected:
	std::vector<Scene*> m_ActiveScenes;
	std::map<std::string, Scene*> m_pScenes;
};