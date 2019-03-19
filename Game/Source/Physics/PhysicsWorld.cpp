#include "GamePCH.h"
#include "PhysicsWorld.h"
#include "Physics/ContactListener.h"
#include "../Scenes/DebugDraw.h"
#include "../GameObjects/Camera.h"

PhysicsWorld::PhysicsWorld()
{
	m_pWorld = nullptr;
	m_pContactListener = nullptr;
	m_Debugdraw = nullptr;
	m_pWorldBody = nullptr;
	m_TotalTime = 0;

}

PhysicsWorld::~PhysicsWorld()
{
	delete m_pWorld;
	delete m_pContactListener;
	delete m_Debugdraw;
}

void PhysicsWorld::Init(b2Vec2 gravity)
{
	m_pWorld = new b2World(gravity);
	m_pContactListener = new ContactListener();
	m_pWorld->SetContactListener(m_pContactListener);

	m_Debugdraw = new DebugDraw();
	m_pWorld->SetDebugDraw(m_Debugdraw);

	b2BodyDef bodyDef;
	m_pWorldBody = m_pWorld->CreateBody(&bodyDef);
}

void PhysicsWorld::Update(float deltatime)
{
	m_TotalTime += deltatime;

	float timestep = 1 / 60.0f;
	bool stepped = false;

	while (m_TotalTime >= timestep)
	{
		m_pWorld->Step(timestep, 8, 3);
		m_TotalTime -= timestep;
		stepped = true;
	}

	if (stepped == false)
		m_pWorld->ClearForces();
}

void PhysicsWorld::Draw(Camera* cam, Material* mat)
{
	ImGui::Checkbox("Debug Draw Shape", &shape);
//	ImGui::Checkbox("Debug Draw Center", &center);
	ImGui::Checkbox("Debug Draw Joint", &joint);
	ImGui::Checkbox("Debug Draw AABB", &aabb);
	//ImGui::Checkbox("Debug Draw Pair", &pair);

	uint32 flags = 0; 
	
	if (shape)
		flags |= b2Draw::e_shapeBit;

	if (center)
		flags |= b2Draw::e_centerOfMassBit;

	if (joint)
		flags |= b2Draw::e_jointBit;

	if (aabb)
		flags |= b2Draw::e_aabbBit;

	if (pair)
		flags |= b2Draw::e_pairBit;

	m_Debugdraw->SetFlags(flags);

	m_Debugdraw->SetCamera(cam);
	m_Debugdraw->SetMaterial(mat);
	m_pWorld->DrawDebugData();
}
