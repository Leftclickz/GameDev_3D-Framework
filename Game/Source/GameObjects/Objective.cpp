#include "GamePCH.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/Objective.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"
#include "Physics/BulletMotionState.h"

Objective::Objective(Scene * pScene, std::string name, Transform transform, Mesh * pMesh, Material * pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
{
}

Objective::~Objective()
{
}

void Objective::ContactStarted(GameObject3D * pOtherObj, vec3 normal)
{
	//if (pOtherObj->GetName() == "Player")
	//{
	//	m_pScene->GetGame()->GetEventManager()->QueueEvent(new GameStateEvent(WinState));
	//}
}

void Objective::ContactEnded(GameObject3D * pOtherObj)
{
}