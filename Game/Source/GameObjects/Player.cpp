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

Player::Player(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject3D(pScene, name, transform, pMesh, pMaterial)
	, m_pPlayerController(nullptr)
	, m_Speed(PLAYER_SPEED)
	, m_TurningSpeed(PLAYER_SPEED)
	, m_Health(PLAYER_HEALTH)
{
	m_Followlight = new FollowLight(m_pScene, "PlayerLight", Transform(vec3(0, 8, 0), vec3(0), vec3(1)), nullptr, nullptr);
	m_Followlight->SetObjectAttachment(this);
	m_Followlight->SetFollowOffset(FOLLOW_LIGHT_OFFSET);
	m_Followlight->SetAttenuationFactor(8.0f);
	m_Followlight->AssignLightColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pScene->AddLightObject(m_Followlight);

	ResourceManager* resources = pScene->GetResourceManager();

	AudioList* deathAudio = resources->AddAudioList("Death Sounds", new WeightedRandomAudioList());
	deathAudio->AddAudio(resources->GetAudio("Player Death 1"));
	deathAudio->AddAudio(resources->GetAudio("Player Death 2"));
	deathAudio->AddAudio(resources->GetAudio("Player Death 3"));
	m_PlayerSounds["Death"] = deathAudio;

	AudioList* spawnAudio = resources->AddAudioList("Spawn Sounds", new WeightedRandomAudioList());
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 1"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 2"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 3"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 4"));
	spawnAudio->AddAudio(resources->GetAudio("Player Spawn 5"));
	m_PlayerSounds["Spawn"] = spawnAudio;
	
}

Player::~Player()
{
}

void Player::Update(float deltatime)
{
	GameObject3D::Update( deltatime );

	for each(std::pair<std::string, AudioList*> list in m_PlayerSounds)
		list.second->Update(deltatime);

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
}

void Player::Jump()
{
	if (m_Body && m_JumpCount > 0)
	{
		m_Body->applyCentralImpulse(btVector3(0.0f, m_JumpHeight, 0.0f));
		m_pPlayerController->RemoveJumpInput();
		m_JumpCount--;
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
}

void Player::Reset()
{
	GameObject3D::Reset();

	m_Health = PLAYER_HEALTH;
	m_Speed = PLAYER_SPEED;
	m_JumpCount = MAX_JUMPS;
}
