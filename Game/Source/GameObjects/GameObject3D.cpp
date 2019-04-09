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
#include "Scenes/BulletScene.h"

//for use with textures
GameObject3D::GameObject3D(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject(pScene, name, transform, pMesh, pMaterial)
{
	m_MotionState = nullptr;
	m_Body = nullptr;
	//m_pScene->AddGameObject(this);
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
	//sync motionstate to body
	if (m_Body)
	{
		btTransform transform = m_Body->getWorldTransform();
		m_MotionState->setWorldTransform(transform);
	}

	GameObject::Update(deltatime);

	//synchronize body to motionstate
	if (m_Body)
	{
		btTransform transformTwo;
		m_MotionState->getWorldTransform(transformTwo);

		m_Body->setWorldTransform(transformTwo);
	}

	DisplayImguiDebugInfo();
}

void GameObject3D::Draw(Camera* cam)
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

			BulletScene* scene = dynamic_cast<BulletScene*>(m_pScene);
			if (scene)
				m_pMesh->SetupUniforms(matrix, WorldRotMat, cam, m_pMaterial, scene->GetLightVector());
			else 
				m_pMesh->SetupUniforms(matrix, WorldRotMat, cam, m_pMaterial);

			m_pMesh->Draw(m_pMaterial->GetShader());
		}
	}
}

void GameObject3D::Reset()
{
	if (m_WasLoadedFromJSON)
	{
		SetRotation(cJSONrot);
		SetPosition(cJSONpos);

		//fully resets the body to its initial loaded state
		if (m_Body)
		{
			btTransform transform = m_Body->getWorldTransform();
			transform.setOrigin(btVector3(cJSONpos.x, cJSONpos.y, cJSONpos.z));
			transform.setRotation(btQuaternion(cJSONrot.y, cJSONrot.x, cJSONrot.z));
			m_Body->setLinearVelocity(btVector3(0, 0, 0));
			m_Body->setWorldTransform(transform);
		}
	}

	GameObject::Reset();
}

void GameObject3D::ContactStarted(GameObject3D * pOtherObj, vec3 normal)
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

void GameObject3D::DisplayImguiDebugInfo()
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
		}
		ImGui::PopID();
		ImGui::End();
	}
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

//creates an infinite plane
//@param normal is the direction, by default up
// @param planeconatant is how displaced the plane is in that direction. 0 is usually good.
void GameObject3D::CreatePlane(vec3 normal /*= vec3(0, 1, 0)*/, float planeconatant /*= 0.0f*/)
{
	//no empty vectors
	assert(normal != vec3(0, 0, 0));

	btCollisionShape* plane = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), btScalar(planeconatant));

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
	// 	SetScale(scale);

	cJSON* jComponents = cJSON_GetObjectItem(obj, "Components");
	HandleComponents(jComponents, manager);
}

void GameObject3D::HandleComponents(cJSON * obj, ResourceManager * manager)
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
		if (strcmp(type, "3D Collision Object") == 0)
		{
			if (m_pMesh == nullptr)
			{
				//store the location of this element and handle it after we definitely have a mesh
				collisionIndex = i;
				continue;
			}
			//if we do have a mesh then handle it now
			HandleCollisionLoad(Component);
		}
		std::string stringtype(type);
		if (stringtype.find("Mesh") != std::string::npos)
		{
			HandleMeshLoad(Component, type, manager);
		}
	}
	delete[] type;

	//if we didn't have a mesh when we checked earlier we should now so handle collision now
	if (collisionIndex != -1)
	{
		Component = cJSON_GetArrayItem(obj, collisionIndex);
		HandleCollisionLoad(Component);
	}
}

void GameObject3D::HandleMeshLoad(cJSON * obj, const char * type, ResourceManager * manager)
{
	if (strcmp(type, "Mesh-Primitive") == 0)
	{
		char* mat = new char[50];
		cJSONExt_GetString(obj, "Material", mat, 50);

		SetMaterial(manager->GetMaterial(mat));
		SetMesh(manager->GetMesh("Plane"));

		delete[] mat;
	}
	else if (strcmp(type, "Mesh-OBJ") == 0)
	{
		char* mat = new char[50];
		cJSONExt_GetString(obj, "Material", mat, 50);
		char* OBJ = new char[50];
		cJSONExt_GetString(obj, "OBJFilename", OBJ, 50);

		std::string name(OBJ);
		name.erase(name.end() - 4, name.end());

		SetMaterial(manager->GetMaterial(mat));

		SetMesh(manager->GetMesh(name));

		delete[] mat;
		delete[] OBJ;
	}
}

void GameObject3D::HandleCollisionLoad(cJSON * obj)
{
	assert(m_pMesh != nullptr);

	char* primitiveType = new char[50];
	cJSONExt_GetString(obj, "Primitive", primitiveType, 50);

	float mass;
	cJSONExt_GetFloat(obj, "Mass", &mass);

	if (strcmp(primitiveType, "Sphere") == 0)
	{
		GetMesh()->Rescale(cJSONscale);
		CreateSphereBody(cJSONscale.x, mass);
	}
	else if (strcmp(primitiveType, "Cube") == 0)
	{
		GetMesh()->Rescale(cJSONscale);
		CreateBoxBody(cJSONscale, mass);
	}
	else if (strcmp(primitiveType, "Static Plane") == 0)
	{
		CreatePlane();
	}
	else if (strcmp(primitiveType, "Convex Hull") == 0)
	{
		CreateConvexHullBody(mass);
	}
	delete[] primitiveType;
}