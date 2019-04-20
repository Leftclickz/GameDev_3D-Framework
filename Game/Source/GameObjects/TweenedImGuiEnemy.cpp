#include "GamePCH.h"
#include "TweenedImGuiEnemy.h"

TweenedImGuiEnemy::TweenedImGuiEnemy(Scene * pScene, std::string name, Transform transform, Mesh * pMesh, Material * pMaterial)
	: Enemy(pScene,name, transform, pMesh, pMaterial)
{
	Scale_Tween_Value = 0.5f;
	Position_Tween_Value = vec3(0);
	Rotation_Tween_Value = vec3(0);
	FunctionToUse = TweenFunc_Linear;
}

void TweenedImGuiEnemy::ImGuiDisplayDebugInfo()
{
	if (isEnabled)
	{
		ImGui::Begin("GameObjects");
		ImGui::PushID(this);
		if (ImGui::CollapsingHeader(m_Name.c_str()))
		{
			ImGui::Text("PositionX: %.3f", m_Position.x);
			ImGui::Text("PositionY: %.3f", m_Position.y);
			ImGui::Text("PositionZ: %.3f", m_Position.z);

			//Tween altering
			if (ImGui::TreeNode("Tweens"))
			{

				//change the tween function used
				if (ImGui::TreeNode("TweenFunction"))
				{
					if (ImGui::Button("Linear"))
						FunctionToUse = TweenFunc_Linear;
					if (ImGui::Button("Sine Ease In"))
						FunctionToUse = TweenFunc_SineEaseIn;
					if (ImGui::Button("Sine Ease Out"))
						FunctionToUse = TweenFunc_SineEaseOut;
					if (ImGui::Button("Sine Ease In/Out"))
						FunctionToUse = TweenFunc_SineEaseInOut;
					if (ImGui::Button("Bounce Ease In"))
						FunctionToUse = TweenFunc_BounceEaseIn;
					if (ImGui::Button("Bounce Ease Out"))
						FunctionToUse = TweenFunc_BounceEaseOut;
					if (ImGui::Button("Bounce Ease In/Out"))
						FunctionToUse = TweenFunc_BounceEaseInOut;
					if (ImGui::Button("Elastic Ease In"))
						FunctionToUse = TweenFunc_ElasticEaseIn;
					if (ImGui::Button("Elastic Ease Out"))
						FunctionToUse = TweenFunc_ElasticEaseOut;
					if (ImGui::Button("Elastic Ease In/Out"))
						FunctionToUse = TweenFunc_ElasticEaseInOut;

					ImGui::TreePop();
				}

				//scale tweens
				if (ImGui::TreeNode("Scale"))
				{
					ImGui::SliderFloat("Scale Value", &Scale_Tween_Value, -1.0f, 1.0f);
					if (ImGui::Button("Apply"))
					{
						AddTween(TweenType_Scale, vec3(Scale_Tween_Value), 1.0f, FunctionToUse);
					}

					ImGui::TreePop();
				}

				//rotation tweens
				if (ImGui::TreeNode("Rotation"))
				{
					ImGui::SliderFloat("Rotation X Value", &Rotation_Tween_Value.x, -90.0f, 90.0f);
					ImGui::SliderFloat("Rotation Y Value", &Rotation_Tween_Value.y, -90.0f, 90.0f);
					ImGui::SliderFloat("Rotation Z Value", &Rotation_Tween_Value.z, -90.0f, 90.0f);

					if (ImGui::Button("Apply"))
					{
						AddTween(TweenType_Rotation, Rotation_Tween_Value, 1.0f, FunctionToUse);
					}

					ImGui::TreePop();
				}

				//translation tweens
				if (ImGui::TreeNode("Translation"))
				{
					ImGui::SliderFloat("Translation X Value", &Position_Tween_Value.x, -10.0f, 10.0f);
					ImGui::SliderFloat("Translation Y Value", &Position_Tween_Value.y, -10.0f, 10.0f);
					ImGui::SliderFloat("Translation Z Value", &Position_Tween_Value.z, -10.0f, 10.0f);

					if (ImGui::Button("Apply"))
					{
						AddTween(TweenType_Translation, Position_Tween_Value, 1.0f, FunctionToUse);
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}
		ImGui::PopID();
		ImGui::End();
	}
}
