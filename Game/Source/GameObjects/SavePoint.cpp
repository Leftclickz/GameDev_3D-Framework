#include "GamePCH.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/SavePoint.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"
#include "Physics/BulletMotionState.h"
#include "Player.h"
#include "BoshyBullet.h"
#include "Gun.h"
#include "ChaseCameraObject.h"

SavePoint::SavePoint(Scene * pScene, std::string name, Transform transform, Mesh * pMesh, Material * pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
{
}

SavePoint::~SavePoint()
{
}

void SavePoint::ContactStarted(GameObject3D * pOtherObj, vec3 normal)
{
	//did a bullet hit me?
	if (pOtherObj->GetName().find("Bullet") != std::string::npos)
	{
		BoshyBullet* bullet = (BoshyBullet*)pOtherObj;

		//the player owns this bullet right?
		if (strcmp(bullet->GetOwner()->GetOwner()->GetName().c_str(), "Player") == 0)
		{
			//Trigger save of locations
			SetRespawnValues();
		}
	}
}

void SavePoint::ContactEnded(GameObject3D * pOtherObj)
{
}

void SavePoint::SetRespawnValues()
{
	Player* player = ((Player*)m_pScene->GetGameObjectByName("Player"));
	player->SetRespawnData(player->GetPosition(), player->GetRotation());

	//set camera respawn rotation
	ChaseCameraObject* cam = ((ChaseCameraObject*)m_pScene->GetGameObjectByName("ChaseCamera"));
	cam->SetRespawnRotation(cam->GetRotation());
}
