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

void LightObject::HandleComponents(cJSON * obj, ResourceManager * manager)
{
	int numComponents = cJSON_GetArraySize(obj);

	cJSON* Component;
	char* type = new char[50];
	int collisionIndex = -1;
	for (int i = 0; i < numComponents; i++)
	{
		Component = cJSON_GetArrayItem(obj, i);

		cJSONExt_GetString(Component, "Type", type, 50);

		//if this string is within the type string (== 0 means success in finding it)
		//Only worry about collision and sprites. Joints are an entire different function.
		if (strcmp(type, "Light") == 0)
		{
			vec4 color(0, 0, 0, 0);
			cJSONExt_GetFloatArray(Component, "Color", &color.x, 4);
			cJSONcolor = color;
			AssignLightColor(color);

			vec3 attenuation(0);
			cJSONExt_GetFloatArray(Component, "Atten", &attenuation.x, 4);
			cJSONatten = attenuation;
			SetAttenuationFactor(attenuation.x);
		}
	}
	delete[] type;
}

void LightObject::Reset()
{
	GameObject3D::Reset();

	AssignLightColor(cJSONcolor);
	SetAttenuationFactor(cJSONatten.x);
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
			if (ImGui::TreeNode("Position"))
			{
				ImGui::SliderFloat("PositionX", &m_Position.x, -25.0f, 25.0f);
				ImGui::SliderFloat("PositionY", &m_Position.y, -25.0f, 25.0f);
				ImGui::SliderFloat("PositionZ", &m_Position.z, -25.0f, 25.0f);
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
