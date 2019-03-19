#include "GamePCH.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Shootable.h"
#include "Bullet.h"
#include "../Game/Game.h"
#include "../Scenes/Scene.h"
#include "Physics/PhysicsWorld.h"

Shootable::Shootable(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject2D(pScene, name, transform, pMesh, pMaterial)
{
}

Shootable::~Shootable()
{
}

//mostly checks if the object goes off screen, if so remove it
void Shootable::Update(float deltatime)
{
	GameObject2D::Update(deltatime);
}

void Shootable::Draw(Camera* cam)
{
	GameObject2D::Draw(cam);
}

void Shootable::BeginContact(b2Fixture * fix, vec2 pos, vec2 normal)
{
	//only die if shot
	if (fix->GetFilterData().categoryBits == Collision_Bullet)
	{
		//add to a pool that will respawn on reset
		m_pScene->TempRemoveGameObject(this);
		//move my collision way off screen
		m_pScene->GetGame()->GetEventManager()->QueueEvent(new TranslateObjectEvent(-30.0f, -500.0f, 0.0f, this));
	}
}

void Shootable::EndContact(GameObject2D * pobj)
{
}