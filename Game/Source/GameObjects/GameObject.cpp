#include "GamePCH.h"

#include "Mesh/Mesh.h"
#include "GameObjects/GameObject.h"
#include "../Scenes/Scene.h"
#include "../Game/Game.h"
#include "Mesh/SpriteSheet.h"
#include "GameObject.h"
#include "Mesh/Material.h"
#include "Camera.h"
#include "Transformable.h"
#include "../Game/ResourceManager.h"
#include "Physics/PhysicsWorld.h"

//for use with textures
GameObject::GameObject(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: Transformable(transform)
	, m_pScene(pScene)
	, m_Name(name)
	, m_pMesh(pMesh)
	, m_pMaterial(pMaterial)
{
}

GameObject::~GameObject()
{
}

void GameObject::Update(float deltatime)
{
	if (isEnabled)
	{

	}
}

void GameObject::Draw(Camera* cam)
{
	if (isEnabled)
	{
		if (m_pMesh != nullptr)
		{
			m_pMesh->SetupAttributes(m_pMaterial->GetShader());

			//worldmatrix
			mat4 matrix;
			matrix.CreateSRT(m_Scale, m_Rotation, m_Position);

			mat4 WorldRotMat;
			WorldRotMat.CreateRotation(m_Rotation);

			m_pMesh->SetupUniforms(matrix, WorldRotMat, cam, m_pMaterial);
			m_pMesh->Draw(m_pMaterial->GetShader());
		}
	}
}

//If object was loaded from cJSON this will reset it to the state it was in when spawned
void GameObject::Reset()
{

}
