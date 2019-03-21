#include "GamePCH.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/Player.h"
#include "GameObjects/PlayerController.h"
#include "Mesh/SpriteSheet.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"

Player::Player(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
	, m_pPlayerController(nullptr)
	, m_Speed(PLAYER_SPEED)
	, m_TurningSpeed(PLAYER_SPEED)
{
}

Player::~Player()
{
}

void Player::Update(float deltatime)
{
	GameObject3D::Update( deltatime );

    vec3 dir( 0, 0, 0 );

    if( m_pPlayerController )
    {
        if( m_pPlayerController->IsHeld_Up() )
        {
            dir.z = 1;
        }

        if( m_pPlayerController->IsHeld_Down() )
        {
            dir.z = -1;
        }

        if( m_pPlayerController->IsHeld_Left() )
        {
            dir.x = -1;
        }

        if( m_pPlayerController->IsHeld_Right() )
        {
            dir.x = 1;
        }

		if (m_pPlayerController->IsHeld_In())
		{
			dir.y = -1;
		}

		if (m_pPlayerController->IsHeld_Out())
		{
			dir.y = 1;
		}
    }

	//if (m_Body)
	//{
	//	//dir.y = -9.8f; //apply gravity

	//	float mass = 1 / m_Body->getInvMass();
	//	btVector3 currentVel = m_Body->getLinearVelocity();

	//	btVector3 VelDiff = btVector3(dir.x - currentVel.x(), dir.y - currentVel.y(), dir.z - currentVel.z());

	//	float timestep = 1 / 60.0f;

	//	btVector3 force = mass * VelDiff;
	//	force.setX(force.x() / timestep);

	//	m_Body->applyCentralImpulse(force);
	//}
	//else
		m_Position += dir * m_Speed * deltatime;
}

void Player::Draw(Camera* cam)
{
	GameObject3D::Draw(cam);

// 	ImGui::Text("RotationX: %.3f", m_Rotation.x);
// 	ImGui::Text("RotationY: %.3f", m_Rotation.y);
// 	ImGui::Text("RotationZ: %.3f", m_Rotation.z);

}