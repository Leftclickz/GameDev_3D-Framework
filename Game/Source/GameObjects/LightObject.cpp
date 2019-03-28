#include "GamePCH.h"
#include "LightObject.h"

LightObject::LightObject(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
{
}

StandardLight LightObject::GetLight()
{
	m_Light.position = GetPosition();

	return m_Light;
}
