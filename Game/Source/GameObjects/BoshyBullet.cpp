#include "GamePCH.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/BoshyBullet.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"
#include "Physics/BulletMotionState.h"
#include "BoshyBullet.h"
#include "../Game/Pool.h"
#include "Gun.h"

BoshyBullet::BoshyBullet(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial, Pool<BoshyBullet*>* pool)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
	, PooledObject(pool)
	, m_Damage(100.0f)
	, m_Speed(5.0f)
	, m_LifeTime(LIFETIME)
{
	SetScale(vec3(2.0f));
	SetMesh(pScene->GetResourceManager()->GetMesh("Bullet"));
	SetMaterial(pScene->GetResourceManager()->GetMaterial("Lighting2"));

	SetPosition(vec3(0, 500000, 0));
	CreateBoxBody(vec3(0.1f, 0.1f, 0.1f), 0.1f);
	m_Body->setGravity(btVector3(0, 0, 0));
	SetEnabled(false);
}

BoshyBullet::~BoshyBullet()
{
}

void BoshyBullet::Update(float delta)
{
	if (isEnabled)
	{
		m_LifeTime -= delta;

		if (m_LifeTime <= 0.0f)
		{
			Deactivate();
		}
	}
}

void BoshyBullet::Draw(Camera* cam)
{
	GameObject3D::Draw(cam);
}

//deal damage to what we hit and then return to pool
//does not damage the owner
void BoshyBullet::ContactStarted(GameObject3D* pOtherObj, vec3 normal)
{
	//did we hit something that isnt a bullet?
	if (pOtherObj->GetName().find("Bullet") == std::string::npos)
	{
		//did we hit something that isn't boshy?
		if (pOtherObj != m_Owner->GetOwner())
		{
			pOtherObj->TakeDamage(m_Damage);
		}

		//we've hit something. go away now.
		Deactivate();
	}
}

void BoshyBullet::Reset()
{
	SetEnabled(false);
	m_LifeTime = LIFETIME;

	btTransform transform;
	m_MotionState->getWorldTransform(transform);
	transform.setOrigin(btVector3(0, 500000, 0));
	m_Body->setWorldTransform(transform);
	SetPosition(vec3(0, 500000, 0));
	m_Body->setLinearVelocity(btVector3(0, 0, 0));
}

void BoshyBullet::Fire()
{
	//Get our players forward direction.
	vec3 dir = GetOwner()->GetOwner()->GetDirection();

	//set our position and activate the body
	vec3 pos = GetOwner()->GetOwner()->GetPosition();
	SetPosition(pos + (dir * 3.0f) + vec3(0,2.0f,0));
	Activate();

	//Wipe the old transforms and set the new location.
	if (m_Body)
	{
		btTransform transform;
		m_MotionState->getWorldTransform(transform);
		transform.setRotation(btQuaternion(dir.x, dir.y, dir.z));

		m_MotionState->setWorldTransform(transform);
		m_Body->setWorldTransform(transform);
	}

	//TODO actually fire it using the owner's forward vector
	btVector3 force = btVector3(dir.x, dir.y, dir.z) * m_Speed;
	m_Body->applyCentralImpulse(force);
}

void BoshyBullet::Deactivate()
{
	SetEnabled(false);
	((Gun*)GetOwner())->ReturnToMag(this);
	m_LifeTime = LIFETIME;

	btTransform transform;

	//sync body to state
	SetPosition(vec3(0, 500000, 0));
	m_MotionState->getWorldTransform(transform);
	m_Body->setWorldTransform(transform);
	m_Body->setLinearVelocity(btVector3(0, 0, 0));
}

void BoshyBullet::Activate()
{
	SetEnabled(true);
	m_Body->activate(true);
}
