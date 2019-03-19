#include "GamePCH.h"
#include "GameObjects/Bullet.h"
#include "Bullet.h"
#include "../Game/Game.h"
#include "../Scenes/Scene.h"

Bullet::Bullet(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject2D(pScene, name, transform, pMesh, pMaterial)
{
}

Bullet::~Bullet()
{
}

//mostly checks if the object goes off screen, if so remove it
void Bullet::Update(float deltatime)
{
	GameObject2D::Update(deltatime);

	if (abs(m_Position.x) > 20.0f || abs(m_Position.y) > 20.0f)
	{
		RemoveSelf();
	}
}

void Bullet::Draw(Camera* cam)
{
	GameObject2D::Draw(cam);
}

//let this know what pool it is in
void Bullet::SetPool(Pool<GameObject*>* pPool)
{
	m_pPoolThisObjectCameFrom = pPool;
}

//sets the bullets position and applies an impulse in the direction
void Bullet::Fire(vec3 pos, EDirection dir)
{
	b2Vec2 direction(1.0f, 0.0f);

	m_pBody->SetGravityScale(0.0f);

	//stop it from moving if it was before it got re-added to the pool
	m_pBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

	if (dir == ELeft)
	{
		direction = b2Vec2(-5.0f, 0.0f);
	}
	else if (dir == ERight)
	{
		direction = b2Vec2(5.0f, 0.0f);
	}

	m_pBody->SetTransform(b2Vec2(pos.x, pos.y), 0.0f);
	m_pBody->ApplyLinearImpulseToCenter(direction, true);
}

//if it hits anything call RemoveSelf
void Bullet::BeginContact(b2Fixture * fix, vec2 pos, vec2 normal)
{
	RemoveSelf();
}

void Bullet::EndContact(GameObject2D * pobj)
{
}

//called whenever a bullet dies basically. Removes it from the scene and queues an event for the scene to move it's body off-screen
void Bullet::RemoveSelf()
{
	if (m_pScene->RemoveGameObject(this))
	{
		m_pScene->GetGame()->GetEventManager()->QueueEvent(new TranslateObjectEvent(0.0f, -500.0f, 0.0f, this));
		m_pPoolThisObjectCameFrom->ReturnObjectToPool(this);
	}
}