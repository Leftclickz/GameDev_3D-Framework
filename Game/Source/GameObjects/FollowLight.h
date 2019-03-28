#pragma once
#include "LightObject.h"

class FollowLight : public LightObject
{
public:

	FollowLight(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);
	
	virtual void SetObjectAttachment(GameObject* obj) { m_ObjectToFollow = obj; }
	virtual StandardLight GetLight() override;

	virtual void SetFollowOffset(vec3 offset) { m_FollowOffset = offset; }

	virtual void DisplayImguiDebugInfo() override;

protected:

	GameObject* m_ObjectToFollow;
	vec3 m_FollowOffset;
};