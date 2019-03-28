#include "GamePCH.h"
#include "FollowLight.h"

FollowLight::FollowLight(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: LightObject(pScene, name, transform, pMesh, pMaterial)
{
	m_ObjectToFollow = nullptr;
	m_FollowOffset = vec3(0);
}

StandardLight FollowLight::GetLight()
{
	if (m_ObjectToFollow != nullptr)
		SetPosition(m_ObjectToFollow->GetPosition() + m_FollowOffset);

	return LightObject::GetLight();
}
