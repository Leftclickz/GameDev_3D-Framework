#pragma once
#include "GameObjects/GameObject3D.h"


struct StandardLight
{
	StandardLight() : position(0), color(vec4(0.0f,0.0f,0.0f,0.0f)), attenuationFactor(0.0f) {}

	vec3 position;
	vec4 color;
	float attenuationFactor;
};


class LightObject : public GameObject3D
{

public:

	LightObject(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual void AssignLightColor(vec4 color) { m_Light.color = color; }
	virtual void SetAttenuationFactor(float factor) { m_Light.attenuationFactor = factor; }

	virtual StandardLight GetLight();

protected:

	StandardLight m_Light;

}; 
