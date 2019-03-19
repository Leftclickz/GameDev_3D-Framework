#include "GamePCH.h"

#include "Mesh/Mesh.h"
#include "GameObjects/GameObject2D.h"
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
GameObject2D::GameObject2D(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject(pScene, name, transform, pMesh, pMaterial)
{
}

GameObject2D::~GameObject2D()
{
}

void GameObject2D::Update(float deltatime)
{
	GameObject::Update(deltatime);

	if (isEnabled)
	{
		if (m_pBody)
		{
			b2Vec2 pos = m_pBody->GetPosition();
			float angle = m_pBody->GetAngle();

			m_Position.x = pos.x;
			m_Position.y = pos.y;
			m_Rotation.z = -angle / PI * 180.0f;
		}
	}
}

void GameObject2D::Draw(Camera* cam)
{
	GameObject::Draw(cam);

	if (isEnabled)
	{
		if (m_pMesh != nullptr)
		{
			m_pMesh->SetupAttributes(m_pMaterial->GetShader());

			//worldmatrix
			mat4 matrix;
			matrix.CreateSRT(m_Scale, m_Rotation, m_Position);

			m_pMesh->SetupUniforms(matrix, cam, m_pMaterial);
			m_pMesh->Draw(m_pMaterial->GetShader());
		}
	}
}

b2Body* GameObject2D::GetBody()
{
	return m_pBody;
}

void GameObject2D::CreateBody(bool isStatic)
{
	b2BodyDef bodydef;
	bodydef.type = isStatic ? b2_staticBody : b2_dynamicBody;
	bodydef.position.Set(m_Position.x, m_Position.y);
	bodydef.angle = -m_Rotation.z / 180.0f * PI;
	bodydef.userData = this;

	m_pBody = m_pScene->Getb2World()->CreateBody(&bodydef);
}

//create a box fixture and add it to the body
void GameObject2D::AddBox(vec2 size, float32 density /*= 1.0f*/, b2Vec2 offset /*= b2Vec2(0.0f, 0.0f)*/, bool isSensor /*= false*/, float32 friction /*= 0.2f*/, float32 restitution /*= 0.0f*/)
{
	b2PolygonShape boxShape;
	boxShape.SetAsBox(size.x, size.y, offset, 0.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = density;
	fixtureDef.isSensor = isSensor;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.userData = this;
	m_pBody->CreateFixture(&fixtureDef);
}

//create a box fixture and add it to the body
void GameObject2D::AddBox(cJSON* jObj)
{
	vec2 size(m_Scale.x * 0.5f, m_Scale.y * 0.5f);

	float32 density = 1.0f;
	cJSONExt_GetFloat(jObj, "Density", &density);

	b2Vec2 offset = b2Vec2(0.0f, 0.0f);
	cJSONExt_GetFloatArray(jObj, "Offset", &offset.x, 2);

	bool isSensor = false;
	cJSONExt_GetBool(jObj, "IsSensor", &isSensor);

	float32 friction = 0.7f;
	cJSONExt_GetFloat(jObj, "Friction", &friction);

	float32 restitution = 0.0f;
	cJSONExt_GetFloat(jObj, "Restitution", &restitution);

	AddBox(size, density, offset, isSensor, friction, restitution);
}

//create a circle fixture and add it to the body
void GameObject2D::AddCircle(float32 radius, b2Vec2 offset /*= b2Vec2(0.0f, 0.0f)*/, float32 density /*= 1.0f*/, bool isSensor /*= false*/, float32 friction /*= 0.2f*/, float32 restitution /*= 0.0f*/)
{
	b2CircleShape circleShape;
	circleShape.m_p.Set(offset.x, offset.y); // Offset relative to the body
	circleShape.m_radius = radius;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = density;
	fixtureDef.isSensor = isSensor;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.userData = this;
	m_pBody->CreateFixture(&fixtureDef);
}

//create a circle fixture and add it to the body
void GameObject2D::AddCircle(cJSON* jObj)
{
	float radius = m_Scale.x * 0.5f;

	float32 density = 1.0f;
	cJSONExt_GetFloat(jObj, "Density", &density);

	b2Vec2 offset = b2Vec2(0.0f, 0.0f);
	cJSONExt_GetFloatArray(jObj, "Offset", &offset.x, 2);

	bool isSensor = false;
	cJSONExt_GetBool(jObj, "IsSensor", &isSensor);

	float32 friction = 0.7f;
	cJSONExt_GetFloat(jObj, "Friction", &friction);

	float32 restitution = 0.0f;
	cJSONExt_GetFloat(jObj, "Restitution", &restitution);

	AddCircle(radius, offset, density, isSensor, friction, restitution);
}

//builds and spawns the joint
void GameObject2D::CreateRevolute(b2Body* pOtherBody, bool isMotorEnabled, float motorSpeed, float motorTorque)
{
	b2RevoluteJointDef jointDef;
	jointDef.Initialize(m_pBody, pOtherBody, b2Vec2(m_Position.x, m_Position.y));
	jointDef.enableMotor = isMotorEnabled;
	jointDef.motorSpeed = motorSpeed;
	jointDef.maxMotorTorque = motorTorque;
	m_pScene->Getb2World()->CreateJoint(&jointDef);
}

//builds and spawns the joint
void GameObject2D::CreatePrismatic(b2Body* pOtherBody, b2Vec2 axis, bool isMotorEnabled, float motorSpeed, float motorMaxForce, bool isLimitEnabled, float LimitMin, float LimitMax)
{
	b2PrismaticJointDef jointDef;
	jointDef.Initialize(m_pBody, pOtherBody, b2Vec2(m_Position.x, m_Position.y), axis);
	jointDef.enableMotor = isMotorEnabled;
	jointDef.motorSpeed = motorSpeed;
	jointDef.enableLimit = isLimitEnabled;
	jointDef.maxMotorForce = motorMaxForce;
	jointDef.lowerTranslation = LimitMin;
	jointDef.upperTranslation = LimitMax;

	m_pScene->Getb2World()->CreateJoint(&jointDef);
}

//gets all the variables for a Prismatic Revolute Joint then creates it
void GameObject2D::AddRevoluteJoint(cJSON* jObj)
{
	bool isMotorEnabled = false;
	cJSONExt_GetBool(jObj, "MotorEnabled", &isMotorEnabled);

	float motorSpeed = 0.0f;
	cJSONExt_GetFloat(jObj, "MotorSpeed", &motorSpeed);

	float motorTorque = 0.0f;
	cJSONExt_GetFloat(jObj, "MotorMaxTorque", &motorTorque);

	char* name = new char[50];
	cJSONExt_GetString(jObj, "OtherGameObject", name, 50);

	GameObject2D* pOtherObj = (GameObject2D*)m_pScene->GetGameObjectByName(name);
	if (pOtherObj)
	{
		CreateRevolute(pOtherObj->GetBody(), isMotorEnabled, motorSpeed, motorTorque);
	}
	else
	{
		CreateRevolute(m_pScene->GetPhysicsWorld()->GetWorldBody(), isMotorEnabled, motorSpeed, motorTorque);
	}

	delete[] name;
}

//gets all the variables for a Prismatic Joint then creates it
void GameObject2D::AddPrismaticJoint(cJSON* jObj)
{
	b2Vec2 axis(0,1);
	cJSONExt_GetFloatArray(jObj, "Up", &axis.x, 2);

	bool isMotorEnabled = false;
	cJSONExt_GetBool(jObj, "MotorEnabled", &isMotorEnabled);

	float motorSpeed = 0.0f;
	cJSONExt_GetFloat(jObj, "MotorSpeed", &motorSpeed);

	float motorMaxForce = 0.0f;
	cJSONExt_GetFloat(jObj, "MotorMaxForce", &motorMaxForce);

	bool isLimitEnabled = false;
	cJSONExt_GetBool(jObj, "LimitEnabled", &isLimitEnabled);

	float Limitmin = 0.0f;
	cJSONExt_GetFloat(jObj, "LimitMin", &Limitmin);

	float Limitmax = 0.0f;
	cJSONExt_GetFloat(jObj, "LimitMax", &Limitmax);

	char* name = new char[50];
	cJSONExt_GetString(jObj, "OtherGameObject", name, 50);
	GameObject2D* pOtherObj = (GameObject2D*)m_pScene->GetGameObjectByName(name);
	if (pOtherObj)
	{
		CreatePrismatic(pOtherObj->GetBody(), axis, isMotorEnabled, motorSpeed, motorMaxForce, isLimitEnabled, Limitmin, Limitmax);
	}
	else
	{
		CreatePrismatic(m_pScene->GetPhysicsWorld()->GetWorldBody(), axis, isMotorEnabled, motorSpeed, motorMaxForce, isLimitEnabled, Limitmin, Limitmax);
	}
	delete[] name;
}

void GameObject2D::AddWeldJoint(cJSON* jObj)
{

}

void GameObject2D::AddEdge()
{
}

void GameObject2D::BeginContact(b2Fixture* fix, vec2 pos, vec2 normal)
{
}

void GameObject2D::EndContact(GameObject2D* pobj)
{

}

//create the object from a cJSON object
//if the object has joints call LoadJointsFromcJSON as well
void GameObject2D::LoadFromcJSON(cJSON* obj, ResourceManager* manager)
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

	cJSON* Component;
	char* type = new char[50];
	for (int i = 0; i < numComponents; i++)
	{
		Component = cJSON_GetArrayItem(jComponents, i);

		cJSONExt_GetString(Component, "Type", type, 50);

		//if this string is within the type string (== 0 means success in finding it)
		//Only worry about collision and sprites. Joints are an entire different function.
		if (strcmp(type, "2D Collision Object") == 0)
		{
			bool isStatic = false;
			cJSONExt_GetBool(Component, "Static", &isStatic);
			CreateBody(isStatic);
			GetBody()->SetGravityScale(0.8f);

			char* primitiveType = new char[50];
			cJSONExt_GetString(Component, "PrimitiveType", primitiveType, 50);

			if (strcmp(primitiveType, "Box") == 0)
			{
				AddBox(Component);
			}
			else if (strcmp(primitiveType, "Circle") == 0)
			{
				AddCircle(Component);
			}

			delete[] primitiveType;
		}
		else if (strcmp(type, "Sprite") == 0)
		{
			char* mat = new char[50];
			cJSONExt_GetString(Component, "Material", mat, 50);
			SetMaterial(manager->GetMaterial(mat));
			SetMesh(manager->GetMesh("Box")); //manually just set the standard 2D Texture Mesh
			delete[] mat;
		}
	}
	delete[] type;
}

//checks if the object is supposed to have joints, if so creates them
void GameObject2D::LoadJointsFromcJSON(cJSON* obj)
{
	cJSON* jComponents = cJSON_GetObjectItem(obj, "Components");

	int numComponents = cJSON_GetArraySize(jComponents);

	cJSON* Component;
	char* type = new char[50];
	for (int i = 0; i < numComponents; i++)
	{
		Component = cJSON_GetArrayItem(jComponents, i);

		cJSONExt_GetString(Component, "Type", type, 50);

		//if this string is within the type string (== 0 means success in finding it)
		//Only worry about collision and sprites. Joints are an entire different function.
		std::string stringtype(type);
		if (stringtype.find("Joint") != std::string::npos)
		{
			HandleJoints(Component);
		}
	}
	delete[] type;
}

//determines what type of joint the object has and builds that type
void GameObject2D::HandleJoints(cJSON* obj)
{
	char* jointType = new char[50];
	cJSONExt_GetString(obj, "Type", jointType, 50);
	std::string stringtype(jointType);

	//check for revolute joints
	if (stringtype.find("Revolute") != std::string::npos)
	{
		AddRevoluteJoint(obj);
	}
	if (stringtype.find("Prismatic") != std::string::npos)
	{
		AddPrismaticJoint(obj);
	}
	if (stringtype.find("Weld") != std::string::npos)
	{
		AddWeldJoint(obj);
	}

	delete[] jointType;
}

//If object was loaded from cJSON this will reset it to the state it was in when spawned
void GameObject2D::Reset()
{
	if (m_WasLoadedFromJSON)
	{
		m_Position = cJSONpos;
		m_Rotation = cJSONrot;
		float rotation = atan2f(m_Rotation.z, m_Rotation.x);

		m_pBody->SetTransform(b2Vec2(m_Position.x, m_Position.y), -m_Rotation.z / 180.0f * PI);
		//m_pBody->SetLinearVelocity(b2Vec2(0.f, 0.f));

		SetRotation(cJSONrot);
		SetPosition(cJSONpos);
		SetScale(cJSONscale);

		//Joints
		if (m_pBody->GetJointList() != nullptr)
		{
			b2JointEdge* jointlist = m_pBody->GetJointList();
			b2Joint* joint = jointlist->joint;

			if (joint->GetType() == e_prismaticJoint)
			{
				m_pBody->SetType(b2_staticBody);
			}
		}
	}

	GameObject::Reset();
}
