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
	m_pScene->AddGameObject(this);
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
			ImGui::Begin("GameObjects");
			ImGui::PushID(this);
			if (ImGui::CollapsingHeader(m_Name.c_str()))
			{
				ImGui::Text("PositionX: %.3f", m_Position.x);
				ImGui::Text("PositionY: %.3f", m_Position.y);
				ImGui::Text("PositionZ: %.3f", m_Position.z);
			}
			ImGui::PopID();
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
	if (m_WasLoadedFromJSON)
	{
		m_Position = cJSONpos;
		m_Rotation = cJSONrot;
		float rotation = atan2f(m_Rotation.z, m_Rotation.x);

		//m_pBody->SetTransform(b2Vec2(m_Position.x, m_Position.y), -m_Rotation.z / 180.0f * PI);
		//m_pBody->SetLinearVelocity(b2Vec2(0.f, 0.f));

		SetRotation(cJSONrot);
		SetPosition(cJSONpos);
		SetScale(cJSONscale);
	}

	GameObject::Reset();
}

void GameObject3D::ContactStarted(GameObject3D * pOtherObj)
{
}

void GameObject3D::ContactEnded(GameObject3D * pOtherObj)
{
}

#undef new
//Called by all body types to actually create the body
void GameObject3D::CreateBody(btCollisionShape* shape, float mass)
{
	//m_pScene->GetBulletManager()->collisionShapes.push_back(shape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, 0, 0));
	groundTransform.setRotation(btQuaternion(btScalar(2.0f), btScalar(0.0f), btScalar(0.0f)));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	m_MotionState = new BulletMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_MotionState, shape, localInertia);
	m_Body = new btRigidBody(rbInfo);
	m_Body->setUserPointer(this);

	//add the body to the dynamics world
	m_pScene->GetBulletManager()->dynamicsWorld->addRigidBody(m_Body);
	m_pScene->Add3DBody(m_Body);
}

//Creates a Box Rigid Body for this object by default mass is 0 meaning the object is static
void GameObject3D::CreateBoxBody(vec3 size, float mass /*= 0.0f*/)
{
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(size.x), btScalar(size.y), btScalar(size.z)));

	CreateBody(groundShape, mass);
}

//Creates a Sphere Rigid Body for this object by default mass is 0 meaning the object is static
void GameObject3D::CreateSphereBody(float radius, float mass /*= 0.0f*/)
{
	btCollisionShape* sphereShape = new btSphereShape(radius);

	CreateBody(sphereShape, mass);

}

//Creates a Rigid Body for this object by default mass is 0 meaning the object is static
//Use this for any OBJ built objects. If it is not created with OBJ this will create nothing
void GameObject3D::CreateConvexHullBody(float mass /*= 0.0f*/)
{
	btCollisionShape* hullShape = new btConvexHullShape();

	std::vector<vec3>points = m_pMesh->GetOBJVerts();

	for (unsigned int i = 0; i < points.size(); i++)
	{
		((btConvexHullShape*)hullShape)->addPoint(btVector3(points[i].x, points[i].y, points[i].z));
	}

	CreateBody(hullShape, mass);
}

//creates an infinite plane for a floor at 0,0,0
void GameObject3D::CreatePlane()
{
	btVector3 normal(0, 1, 0);
	btCollisionShape* plane = new btStaticPlaneShape(normal, btScalar(0.0f));

	CreateBody(plane, 0.0f);
}

void GameObject3D::LoadFromcJSON(cJSON* obj, ResourceManager* manager)
{
	m_WasLoadedFromJSON = true;

	char* name = new char[50];
	cJSONExt_GetString(obj, "Name", name, 50);
	m_Name = name;
	delete[] name;

	vec3 pos(0);
	cJSONExt_GetFloatArray(obj, "Pos", &pos.x, 3);
	cJSONpos = pos;
	SetPosition(pos);

	vec3 rot(0);
	cJSONExt_GetFloatArray(obj, "Rot", &rot.x, 3);
	cJSONrot = rot;
	SetRotation(rot);

	vec3 scale(0);
	cJSONExt_GetFloatArray(obj, "Scale", &scale.x, 3);
	cJSONscale = scale;
	SetScale(scale);

	cJSON* jComponents = cJSON_GetObjectItem(obj, "Components");

	int numComponents = cJSON_GetArraySize(jComponents);

// 	cJSON* Component;
// 	char* type = new char[50];
// 	for (int i = 0; i < numComponents; i++)
// 	{
// 		Component = cJSON_GetArrayItem(jComponents, i);
// 
// 		cJSONExt_GetString(Component, "Type", type, 50);
// 
// 		//if this string is within the type string (== 0 means success in finding it)
// 		//Only worry about collision and sprites. Joints are an entire different function.
// 		if (strcmp(type, "3D Collision Object") == 0)
// 		{
// 			bool isStatic = false;
// 			cJSONExt_GetBool(Component, "Static", &isStatic);
// 			CreateBody(isStatic);
// 
// 			char* primitiveType = new char[50];
// 			cJSONExt_GetString(Component, "PrimitiveType", primitiveType, 50);
// 
// 			if (strcmp(primitiveType, "Box") == 0)
// 			{
// 				AddBox(Component);
// 			}
// 			else if (strcmp(primitiveType, "Circle") == 0)
// 			{
// 				AddCircle(Component);
// 			}
// 
// 			delete[] primitiveType;
// 		}
// 		else if (strcmp(type, "Sprite") == 0)
// 		{
// 			char* mat = new char[50];
// 			cJSONExt_GetString(Component, "Material", mat, 50);
// 			SetMaterial(manager->GetMaterial(mat));
// 			SetMesh(manager->GetMesh("Box")); //manually just set the standard 2D Texture Mesh
// 			delete[] mat;
// 		}
// 	}
// 	delete[] type;
}