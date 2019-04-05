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
	//TODO: Make the light follow the player's rotation.
	//Keep its relative offset based on the player rotating
	if (m_ObjectToFollow != nullptr)
		SetPosition(m_ObjectToFollow->GetPosition() + m_FollowOffset);

	return LightObject::GetLight();
}

void FollowLight::DisplayImguiDebugInfo()
{
	ImGui::Begin("Lights");
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader(m_Name.c_str()))
	{
		ImGui::Checkbox("Enabled", &isEnabled);

		if (isEnabled == true)
		{
			if (ImGui::TreeNode("Position"))
			{
				ImGui::Text("X: %.3f", m_Position.x);
				ImGui::Text("Y: %.3f", m_Position.y);
				ImGui::Text("Z: %.3f", m_Position.z);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Offset"))
			{
				ImGui::SliderFloat("PositionX", &m_FollowOffset.x, -25.0f, 25.0f);
				ImGui::SliderFloat("PositionY", &m_FollowOffset.y, -25.0f, 25.0f);
				ImGui::SliderFloat("PositionZ", &m_FollowOffset.z, -25.0f, 25.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Light"))
			{
				ImGui::SliderFloat("Attenuation", &m_Light.attenuationFactor, 0.0f, 100.0f);
				ImGui::SliderFloat("Ambience", &m_Light.ambienceCoefficient, 0.0f, 1.0f);
				ImGui::ColorEdit3("Color", &m_Light.color.x);
				ImGui::TreePop();
			}
		}
	}

	ImGui::PopID();
	ImGui::End();
}
