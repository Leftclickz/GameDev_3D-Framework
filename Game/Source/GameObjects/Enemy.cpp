#include "GamePCH.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/Enemy.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"
#include "Physics/BulletMotionState.h"

Enemy::Enemy(Scene * pScene, std::string name, Transform transform, Mesh * pMesh, Material * pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
	, m_Speed(10.0f)
	, m_Health(100.0f)
	, m_Damage(100.0f)
{
}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltatime)
{
}

void Enemy::ContactStarted(GameObject3D * pOtherObj, vec3 normal)
{
	pOtherObj->TakeDamage(m_Damage);
}

void Enemy::ContactEnded(GameObject3D * pOtherObj)
{
}