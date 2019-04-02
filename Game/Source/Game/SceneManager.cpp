#include "GamePCH.h"
#include "SceneManager.h"
#include "../Scenes/Scene.h"
#include "../Framework/Source/Events/SceneChangeEvent.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	for (auto object : m_pScenes)
		delete object.second;

	m_ActiveScenes.clear();
	m_pScenes.clear();
}

//only update active scenes starting with the topmost non-transparent scene
void SceneManager::Update(float delta)
{
	if (m_ActiveScenes.size() > 0)
	{
		unsigned int startingIndex = 0;

		for (unsigned int i = m_ActiveScenes.size() - 1; i >= 0; i--)
		{
			startingIndex = i;
			if (!m_ActiveScenes[i]->IsTransparent())
			{
				//if this Scene is not transparent we don't need to consider anything behind it
				break;
			}
		}

		//Update scenes starting with the lowest non-transparent scene
		for (unsigned int j = startingIndex; j < m_ActiveScenes.size(); j++)
		{
			m_ActiveScenes[j]->Update(delta);
		}
	}
}

//only draw active scenes starting with the topmost non-transparent scene
void SceneManager::Draw()
{
	if (m_ActiveScenes.size() > 0)
	{
		unsigned int startingIndex = 0;

		for (unsigned int i = m_ActiveScenes.size() - 1; i >= 0; i--)
		{
			startingIndex = i;
			if (!m_ActiveScenes[i]->IsTransparent())
			{
				//if this Scene is not transparent we don't need to consider anything behind it
				break;
			}
		}

		//draw scenes starting with the lowest non-transparent scene
		for (unsigned int j = startingIndex; j < m_ActiveScenes.size(); j++)
		{
			m_ActiveScenes[j]->Draw();
		}
	}
}

bool SceneManager::PushScene(std::string sceneName)
{
	auto it = m_pScenes.find(sceneName);
	if (it == m_pScenes.end())
		return false;

	for (auto a : m_ActiveScenes)
	{
		if (a == it->second)
		{
			return false;
		}
	}

	m_ActiveScenes.push_back(it->second);
	return true;
}

void SceneManager::Pop()
{
	m_ActiveScenes.pop_back();
}

void SceneManager::PopAllScenes()
{
	for (unsigned int i = 0; i < m_ActiveScenes.size(); i++)
	{
		Pop();
	}
}

bool SceneManager::AddScene(std::string name, Scene* pScene)
{
	if (m_pScenes.find(name) == m_pScenes.end())
	{
		m_pScenes[name] = pScene;
		pScene->LoadContent();
		return true;
	}
	else
		delete pScene;

	return false;
}

void SceneManager::OnEvent(Event* pEvent)
{
	//Scene changing event handling
	if (pEvent->GetEventType() == EventType_SceneChange)
	{
		SceneChangeEvent* sceneEvent = (SceneChangeEvent*)pEvent;

		if (sceneEvent)
		{
			Scene* Previous = reinterpret_cast<Scene*>(sceneEvent->GetFrom());
			Scene* Next = reinterpret_cast<Scene*>(sceneEvent->GetTo());

			if (Previous)	Previous->HasLeftFocus();
			if (Next)		Next->HasEnteredFocus();
		}
	}

	for (auto object : m_ActiveScenes)
	{
		object->OnEvent(pEvent);
	}
}