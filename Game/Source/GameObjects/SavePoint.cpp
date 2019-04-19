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

SavePoint::SavePoint(Scene * pScene, std::string name, Transform transform, Mesh * pMesh, Material * pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
{
}

SavePoint::~SavePoint()
{
}

void SavePoint::ContactStarted(GameObject3D * pOtherObj, vec3 normal)
{
	BoshyBullet* bullet = reinterpret_cast<BoshyBullet*>(pOtherObj);

	if (bullet)
	{
		Player* player = reinterpret_cast<Player*>(bullet->GetOwner()->GetOwner());

		if (player)
		{
			player->SetRespawnData(player->GetPosition(), player->GetRotation());
		}
	}
}

void SavePoint::ContactEnded(GameObject3D * pOtherObj)
{
}