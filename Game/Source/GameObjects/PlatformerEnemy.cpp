#include "GamePCH.h"
#include "GameObjects/GameObject2D.h"
#include "GameObjects/PlatformerEnemy.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/PlayerController.h"
#include "Mesh/SpriteSheet.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"

PlatformerEnemy::PlatformerEnemy(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject2D(pScene, name, transform, pMesh, pMaterial)
{
}

PlatformerEnemy::~PlatformerEnemy()
{
}

void PlatformerEnemy::Update(float deltatime)
{
	GameObject2D::Update(deltatime);
}

void PlatformerEnemy::Draw(Camera* cam)
{
	GameObject2D::Draw(cam);
}