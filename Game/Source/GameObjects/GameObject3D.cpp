#include "GamePCH.h"

#include "Mesh/Mesh.h"
#include "GameObjects/GameObject3D.h"
#include "../Scenes/Scene.h"
#include "../Game/Game.h"
#include "GameObject3D.h"
#include "Mesh/Material.h"
#include "Camera.h"
#include "Transformable.h"
#include "../Game/ResourceManager.h"
#include "Physics/PhysicsWorld.h"
#include "Game/BulletManager.h"
#include "Physics/BulletMotionState.h"

//for use with textures
GameObject3D::GameObject3D(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject(pScene, name, transform, pMesh, pMaterial)
{
	m_MotionState = nullptr;
	m_Body = nullptr;
}

GameObject3D::~GameObject3D()
{
	if (m_MotionState)
	{
		delete m_MotionState;
	}

	if (m_Body)
	{
		m_pScene->GetBulletManager()->dynamicsWorld->removeRigidBody(m_Body);
		delete m_Body;
	}
}

void GameObject3D::OnEvent(Event* pEvent)
{
	GameObject::OnEvent(pEvent);
}

void GameObject3D::Update(float deltatime)
{
	GameObject::Update(deltatime);

	if (isEnabled)
	{
		if (m_Body)
		{
			ImGui::Begin("GameObject3D");
			ImGui::Text("PositionX: %.3f", m_Position.x);
			ImGui::Text("PositionY: %.3f", m_Position.y);
			ImGui::Text("PositionZ: %.3f", m_Position.z);
			ImGui::End();
		}
	}
}

void GameObject3D::Draw(Camera* cam)
{
	GameObject::Draw(cam);
}

void GameObject3D::Reset()
{
	GameObject::Reset();
}

void GameObject3D::ContactStarted(GameObject3D * pOtherObj)
{
}

void GameObject3D::ContactEnded(GameObject3D * pOtherObj)
{
}

#undef new
//Creates a Rigid Body for this object by default mass is 0 meaning the object is static
void GameObject3D::CreateBoxBody(vec3 size, float mass /*= 0.0f*/)
{
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(size.x), btScalar(size.y), btScalar(size.z)));

	//m_pScene->GetBulletManager()->collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, 0, 0));
	groundTransform.setRotation(btQuaternion(btScalar(2.0f), btScalar(0.0f), btScalar(0.0f)));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		groundShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_Rotation.x += 3.0f;
	m_MotionState = new BulletMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, groundShape, localInertia);
	m_Body = new btRigidBody(rbInfo);
	m_Body->setUserPointer(this);

	//add the body to the dynamics world
	m_pScene->GetBulletManager()->dynamicsWorld->addRigidBody(m_Body);
}

void GameObject3D::CreateSphereBody(float radius, float mass /*= 0.0f*/)
{
	btCollisionShape* sphereShape = new btSphereShape(radius);

	//m_pScene->GetBulletManager()->collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, 0, 0));
	groundTransform.setRotation(btQuaternion(btScalar(2.0f), btScalar(0.0f), btScalar(0.0f)));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		sphereShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_Rotation.x += 3.0f;
	m_MotionState = new BulletMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, sphereShape, localInertia);
	m_Body = new btRigidBody(rbInfo);
	m_Body->setUserPointer(this);

	//add the body to the dynamics world
	m_pScene->GetBulletManager()->dynamicsWorld->addRigidBody(m_Body);
}

void GameObject3D::CreateTriangleBody(float mass /*= 0.0f*/)
{
// 	btCollisionShape* sphereShape = new btTriangleShape();
// 
// 		//m_pScene->GetBulletManager()->collisionShapes.push_back(groundShape);
// 
// 		btTransform groundTransform;
// 	groundTransform.setIdentity();
// 	groundTransform.setOrigin(btVector3(0, 0, 0));
// 	groundTransform.setRotation(btQuaternion(btScalar(2.0f), btScalar(0.0f), btScalar(0.0f)));
// 
// 	//rigidbody is dynamic if and only if mass is non zero, otherwise static
// 	bool isDynamic = (mass != 0.f);
// 
// 	btVector3 localInertia(0, 0, 0);
// 	if (isDynamic)
// 		sphereShape->calculateLocalInertia(mass, localInertia);
// 
// 	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
// 	m_Rotation.x += 3.0f;
// 	m_MotionState = new BulletMotionState(this);
// 	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, sphereShape, localInertia);
// 	m_Body = new btRigidBody(rbInfo);
// 	m_Body->setUserPointer(this);
// 
// 	//add the body to the dynamics world
// 	m_pScene->GetBulletManager()->dynamicsWorld->addRigidBody(m_Body);
}

//creates an infinite plane for a floor at 0,0,0
void GameObject3D::CreatePlane()
{
	btVector3 normal(0, 1, 0);
	btCollisionShape* plane = new btStaticPlaneShape(normal, btScalar(0.0f));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, 0, 0));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (0 != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		plane->calculateLocalInertia(0, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_MotionState = new BulletMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, m_MotionState, plane, localInertia);
	m_Body = new btRigidBody(rbInfo);
	m_Body->setUserPointer(this);

	//add the body to the dynamics world
	m_pScene->GetBulletManager()->dynamicsWorld->addRigidBody(m_Body);
}