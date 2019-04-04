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
#include "TweenObject.h"

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
	for (auto tween : m_Tweens)
		delete tween;
}

void GameObject::Update(float deltatime)
{
	if (isEnabled)
	{
		for (auto tween : m_Tweens)
			tween->Update(deltatime);
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

void GameObject::AddTween(TweenType type, vec3 endgoal, float duration, TweenFunc TweenFunction)
{
	TweenObject* Tween = nullptr;

	vec3* object_to_tween;

	switch (type)
	{
	case TweenType_Default:
		return;
		break;
	case TweenType_Scale:
		object_to_tween = &m_Scale;
		break;
	case TweenType_Rotation:
		object_to_tween = &m_Rotation;
		break;
	case TweenType_Translation:
		object_to_tween = &m_Scale;
		break;
	default:
		return;
		break;
	}

	Tween = new TweenObject(*object_to_tween, endgoal, duration, TweenFunction, this, &GameObject::OnTweenEnded);
	m_Tweens.push_back(Tween);
}

void GameObject::OnTweenEnded(TweenObject* obj)
{
	if (obj != nullptr)
	{
		//remove from vector
		auto iteratorForObject = std::find(m_Tweens.begin(), m_Tweens.end(), obj);

		if (iteratorForObject != m_Tweens.end())
		{
			delete obj;
			m_Tweens.erase(iteratorForObject);
		}
	}
}

//If object was loaded from cJSON this will reset it to the state it was in when spawned
void GameObject::Reset()
{

}
