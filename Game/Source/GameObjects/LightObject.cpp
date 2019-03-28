#include "GamePCH.h"
#include "LightObject.h"

LightObject::LightObject(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
{
}

void LightObject::Update(float deltatime)
{
	GameObject3D::Update(deltatime);
}

StandardLight LightObject::GetLight()
{
	m_Light.position = GetPosition();

	return m_Light;
}

void LightObject::DisplayImguiDebugInfo()
{

	ImGui::Begin("Lights");
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(m_Name.c_str()))
	{
		ImGui::Checkbox("Enabled", &isEnabled);

		if (isEnabled == true)
		{
			ImGui::SliderFloat("PositionX", &m_Position.x, -25.0f, 25.0f);
			ImGui::SliderFloat("PositionY", &m_Position.y, -25.0f, 25.0f);
			ImGui::SliderFloat("PositionZ", &m_Position.z, -25.0f, 25.0f);

			ImGui::SliderFloat("Attenuation", &m_Light.attenuationFactor, 0.0f, 1.0f);
			ImGui::SliderFloat("Ambience", &m_Light.ambienceCoefficient, 0.0f, 1.0f);
			ImGui::ColorEdit3("Color", &m_Light.color.x);
		}
	}

	ImGui::PopID();
	ImGui::End();

}
