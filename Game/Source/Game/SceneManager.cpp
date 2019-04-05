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
		//Update starting with the topmost scene.
		for (int i = m_ActiveScenes.size() - 1; i >= 0; i--)
		{
			m_ActiveScenes[i]->Update(delta);

			//if the last scene told us to stop updating, stop
 			if (m_ActiveScenes[i]->DoesPause())
 				break;

			//if the last scene we updated was not transparent don't update whats behind it
			if (!m_ActiveScenes[i]->IsTransparent())
				break;
		}
	}

	if (m_ActiveScenes.size() > 0)
	{
		ImGui::Begin("SceneManager");
		ImGui::PushID(this);
		for (int i = m_ActiveScenes.size() - 1; i >= 0; i--)
		{
			ImGui::CollapsingHeader(m_ActiveScenes[i]->GetName().c_str());
		}
		ImGui::PopID();
		ImGui::End();
	}
}

//only draw active scenes starting with the topmost non-transparent scene
void SceneManager::Draw()
{
	if (m_ActiveScenes.size() > 0)
	{
		unsigned int startingIndex = 0;

		for (int i = m_ActiveScenes.size() - 1; i >= 0; i--)
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
	m_ActiveScenes.back()->HasEnteredFocus();
	return true;
}

void SceneManager::Pop()
{
	m_ActiveScenes.pop_back();
}

void SceneManager::PopAllScenes()
{
	for (auto i : m_ActiveScenes)
	{
		Pop();
	}
}

bool SceneManager::AddScene(std::string name, Scene* pScene)
{
	if (m_pScenes.find(name) == m_pScenes.end())
	{
		m_pScenes[name] = pScene;
		pScene->SetSceneManager(this);
		pScene->LoadContent();
		return true;
	}
	else
		delete pScene;

	return false;
}

bool SceneManager::IsSceneAdded(std::string name)
{
	if (m_pScenes.find(name) == m_pScenes.end())
		return false;

	return true;
}

//returns nullptr if scene not found
Scene* SceneManager::GetSceneByName(std::string name)
{
	if (IsSceneAdded(name))
		return m_pScenes[name];

	return nullptr;

	
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

	if (m_ActiveScenes.size() > 0)
	{
		//give the event to the topmost scene and go down if needed
		for (int i = m_ActiveScenes.size() - 1; i >= 0; i--)
		{
			if (m_ActiveScenes[i]->DoesStopEvents())
			{
				m_ActiveScenes[i]->OnEvent(pEvent);
				break;
			}
			else
			{
				m_ActiveScenes[i]->OnEvent(pEvent);
			}
		}
	}
}

//fully resets the manager, Popping all Scenes
void SceneManager::Reset()
{
	for (auto i : m_ActiveScenes)
	{
		i->Reset();
	}

	PopAllScenes();
}
