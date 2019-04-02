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

	btVector3 dir( 0, 0, 0 );

    if( m_pPlayerController )
    {
		if (m_pPlayerController->IsPressed_Jump())
		{
			Jump();
		}

        if( m_pPlayerController->IsHeld_Up() )
        {
            dir.setZ(1);
        }

        if( m_pPlayerController->IsHeld_Down() )
        {
			dir.setZ(-1);
        }

        if( m_pPlayerController->IsHeld_Left() )
        {
			dir.setX(-1);
        }

        if( m_pPlayerController->IsHeld_Right() )
        {
			dir.setX(1);
        }

		if (m_pPlayerController->IsHeld_In())
		{
			dir.setY(-1);
		}

		if (m_pPlayerController->IsHeld_Out())
		{
			dir.setY(1);
		}

		GameObject* cam = m_pScene->GetGameObjectByName("ChaseCamera");
		if (cam)
		{
			vec3 camrot = cam->GetRotation() / 180.0f * PI;
			vec3 radpos = m_Position / 180 * PI;
			m_Rotation.y = camrot.y / PI * 180;

			if (dir.x() != 0 || dir.z() != 0)
			{
				float ang = atan2f(dir.z(), dir.x());
				ang += camrot.y;
				dir = btVector3(cosf(ang), dir.y(), sinf(ang));
			}
		}
    }

 	if (m_Body)
 	{
 		float mass = 1 / m_Body->getInvMass();
 		btVector3 currentVel = m_Body->getLinearVelocity();
		//currentVel.setY(currentVel.y() - m_Body->getGravity().y());
 
		btVector3 VelDiff = (dir * m_Speed) - currentVel;

 		float timestep = 1 / 60.0f;

		btVector3 force = mass * VelDiff;

		force.setX((force.x() / timestep));
		force.setZ((force.z() / timestep));
 
		m_Body->activate(true);
 		m_Body->applyCentralForce(force);
 	}
}

void Player::Draw(Camera* cam)
{
	GameObject3D::Draw(cam);
}

void Player::Jump()
{
	if (m_Body)
	{
		m_Body->applyCentralImpulse(btVector3(0.0f, 10.0f, 0.0f));
		m_pPlayerController->RemoveJumpInput();
	}
}

void Player::ContactStarted(GameObject3D* pOtherObj)
{
	GameObject3D::ContactStarted(pOtherObj);

	std::string stringtype(pOtherObj->GetName());
	//Push boxes
	if (stringtype.find("Box") != std::string::npos)
	{
		vec3 dir = pOtherObj->GetPosition() - m_Position;
		dir.Normalize();

		dir *= 50.0f;

		pOtherObj->GetBody()->applyCentralImpulse(btVector3(dir.x, dir.y, dir.z));
	}
}

void Player::ContactEnded(GameObject3D* pOtherObj)
{
	GameObject3D::ContactEnded(pOtherObj);
}

void Player::DisplayImguiDebugInfo()
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
			ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 20.0f);
		}
		ImGui::PopID();
		ImGui::End();
	}
}
