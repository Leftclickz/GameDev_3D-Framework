#include "GamePCH.h"
#include "HUD_TextObject.h"
#include "Mesh/TextMesh.h"


HUD_TextObject::HUD_TextObject(Scene* pScene, std::string name, Transform transform, TextMesh* pMesh, Material* pMaterial)
	: GameObject(pScene,name,transform, nullptr, pMaterial)
{
	m_TextMesh = pMesh;
	m_TextMesh->SetMaterial(pMaterial);
	m_pMesh = m_TextMesh;
}

HUD_TextObject::~HUD_TextObject()
{
	if (m_TextMesh != nullptr)
		delete m_TextMesh;
	m_TextMesh = nullptr;
}

void HUD_TextObject::SetString(std::string string)
{
	m_TextMesh->SetText(string);
}

std::string HUD_TextObject::GetString()
{
	return m_TextMesh->GetText();
}

void HUD_TextObject::Draw(Camera* camera)
{
	if (IsVisible())
		GameObject::Draw(camera);
}

