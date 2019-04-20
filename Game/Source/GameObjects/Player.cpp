#include "GamePCH.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/Player.h"
#include "GameObjects/PlayerController.h"
#include "Mesh/SpriteSheet.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"
#include "Physics/BulletMotionState.h"
#include "FollowLight.h"
#include "Gun.h"
#include "Game/BulletManager.h"
#include "SavePoint.h"

Player::Player(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
	, m_pPlayerController(nullptr)
	, m_Speed(PLAYER_SPEED)
	, m_Health(PLAYER_HEALTH)
	, m_RespawnPoint(vec3(0))
{
	m_Followlight = new FollowLight(m_pScene, "PlayerLight", Transform(vec3(0, 8, 0), vec3(0), vec3(1)), nullptr, nullptr);
	m_Followlight->SetObjectAttachment(this);
	m_Followlight->SetFollowOffset(FOLLOW_LIGHT_OFFSET);
	m_Followlight->SetAttenuationFactor(8.0f);
	m_Followlight->AssignLightColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pScene->AddLightObject(m_Followlight);

	ResourceManager* resources = pScene->GetResourceManager();

	AudioList* deathAudio = resources->AddAudioList("Death Sounds", new WeightedRandomAudioList(resources->GetAudio("Player Death 1")->GetWaveFormat()));
	deathAudio->AddAudio(resources->GetAudio("Player Death 1"));
	deathAudio->AddAudio(resources->GetAudio("Player Death 2"));
	deathAudio->AddAudio(resources->GetAudio("Player Death 3"));
	m_PlayerSounds["Death"] = deathAudio;

	AudioList* spawnAudio = resources->AddAudioList("Spawn Sounds", new WeightedRandomAudioList(resources->GetAudio("Player Spawn 1")->GetWaveFormat()));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 1"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 2"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 3"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 4"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 5"));
	m_PlayerSounds["Spawn"] = spawnAudio;

	m_Gun = new Gun(pScene, "Gun");
	m_Gun->SetOwner(this);
}

Player::~Player()
{
	delete m_Gun;
}

void Player::Update(float deltatime)
{
	GameObject3D::Update( deltatime );
	m_Gun->Update(deltatime);

	for each(std::pair<std::string, AudioList*> list in m_PlayerSounds)
		list.second->Update(deltatime);

	btVector3 dir( 0, 0, 0 );

    if( m_pPlayerController )
    {
		if (m_pPlayerController->IsPressed_Jump())
		{
			Jump();
		}
		if (m_pPlayerController->IsPressed_Shoot())
		{
			m_Gun->Shoot();
			m_pPlayerController->RemoveShootInput();
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

		if (m_pPlayerController->IsPressed_Interact())
		{
			Interact();
			m_pPlayerController->RemoveInteractInput();
		}

		GameObject* cam = m_pScene->GetGameObjectByName("ChaseCamera");
		if (cam)
		{
			vec3 camrot = cam->GetRotation() / 180.0f * PI;
			vec3 radpos = m_Position / 180 * PI;
			m_Rotation.y = camrot.y / PI * 180;

			if (m_Rotation.y > 360)
				m_Rotation.y = 0;

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
		//Movement
 		float mass = 1 / m_Body->getInvMass();
 		btVector3 currentVel = m_Body->getLinearVelocity();
 
		btVector3 VelDiff = (dir * m_Speed) - currentVel;

 		float timestep = 1 / 60.0f;

		btVector3 force = mass * VelDiff;

		force.setX((force.x() / timestep));
		force.setZ((force.z() / timestep));
 
		m_Body->activate(true);
 		m_Body->applyCentralForce(force);

		//Rotate to Camera
		btTransform transform;
		m_MotionState->getWorldTransform(transform);
		transform.setRotation(btQuaternion(btVector3(0, 1, 0), -btScalar(m_Rotation.y / 180.0f * PI)));
		m_Body->setWorldTransform(transform);

		//rotate light to player
 		float radianangle = (-m_Rotation.y) / 180.0f * PI;
 		m_Followlight->SetFollowOffset(vec3(sinf(radianangle) * FOLLOW_LIGHT_OFFSET.z, FOLLOW_LIGHT_OFFSET.y, cosf(radianangle) * FOLLOW_LIGHT_OFFSET.z));
 	}
}

void Player::Draw(Camera* cam)
{
	GameObject3D::Draw(cam);

	m_Gun->Draw(cam);
}

void Player::Jump()
{
	if (m_Body && m_JumpCount > 0)
	{
		btVector3 force = btVector3(0.0f, m_JumpHeight, 0.0f);
		force.setY(force.getY() - m_Body->getLinearVelocity().getY());

		if (force.getY() < m_JumpHeight)
		{
			force.setY(m_JumpHeight);
		}

		m_Body->applyCentralImpulse(force);
		m_pPlayerController->RemoveJumpInput();
		m_JumpCount--;
	}
}

void Player::Interact()
{
	//vec3 forward = GetDirection();
	vec3 offset = vec3(0, 1.5f, 0);

	//get forward vector
	vec3 forward = GetDirection();

	//raycast out to see if we interact with anything
	btCollisionWorld::ClosestRayResultCallback raycast = m_pScene->GetBulletManager()->PerformRaycast(GetPosition() + offset, GetPosition() + offset + forward * 10.0f);

	if (raycast.hasHit())
	{
		GameObject* obj = (GameObject*)raycast.m_collisionObject->getUserPointer();

		if (strstr(obj->GetName().c_str(), "SavePoint"))
		{
			((SavePoint*)obj)->SetRespawnValues();
		}
		if (obj->GetName().find("Objective") != std::string::npos)
			m_pScene->GetGame()->GetEventManager()->QueueEvent(new GameStateEvent(WinState));
	}
}

void Player::ContactStarted(GameObject3D* pOtherObj, vec3 normal)
{
	GameObject3D::ContactStarted(pOtherObj, normal);

	//TODO: Figure out why normals are wrong half the time
	//Hit something from on top
	if (normal.y == -1.0f)
	{
  		m_JumpCount = MAX_JUMPS;
	}

	//killplanes
	if (pOtherObj->GetName().find("Plane") != std::string::npos)
	{
		Die();
	}
}

void Player::ContactEnded(GameObject3D* pOtherObj)
{
	GameObject3D::ContactEnded(pOtherObj);
}

void Player::ImGuiDisplayDebugInfo()
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
			ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 200.0f);

			ImGui::Text("RotationX: %.3f", m_Rotation.x);
			ImGui::Text("RotationY: %.3f", m_Rotation.y);
			ImGui::Text("RotationZ: %.3f", m_Rotation.z);
		}
		ImGui::PopID();
		ImGui::End();
	}
}

void Player::TakeDamage(float amount)
{
	m_Health -= amount;

	if (m_Health <= 0)
	{
		m_Health = 0;
		Die();
	}
}

void Player::Die()
{
	m_pScene->GetGame()->GetEventManager()->QueueEvent(new GameStateEvent(LoseState));
	m_PlayerSounds["Spawn"]->StopAudio();
	m_PlayerSounds["Death"]->PlayAudio();
}

void Player::Reset()
{
	GameObject3D::Reset();
	m_Gun->Reset();

	m_pScene->GetGame()->GetEventManager()->QueueEvent(new GameStateEvent(PlayState));
	m_PlayerSounds["Spawn"]->PlayAudio();
	m_PlayerSounds["Death"]->StopAudio();

	m_Health = PLAYER_HEALTH;
	m_Speed = PLAYER_SPEED;
	m_JumpCount = MAX_JUMPS;

	if (m_Body)
	{
		SetRotation(m_RespawnRot);
		SetPosition(m_RespawnPoint);

		//respawn at the actual respawnpoint
		btTransform transform;
		m_MotionState->getWorldTransform(transform);
		m_Body->setWorldTransform(transform);
	}
}

vec3 Player::GetDirection()
{
	GameObject* cam = m_pScene->GetGameObjectByName("ChaseCamera");
	mat4 rotationMatrix;
	rotationMatrix.CreateRotation(cam->GetRotation());

	return rotationMatrix.GetAt();
}
