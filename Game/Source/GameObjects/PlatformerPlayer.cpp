#include "GamePCH.h"
#include "GameObjects/PlatformerPlayer.h"

#include "Game/Game.h"
#include "Mesh/Mesh.h"
#include "GameObjects/PlayerController.h"
#include "Mesh/SpriteSheet.h"
#include "../Scenes/Scene.h"
#include "../Physics/PhysicsWorld.h"
#include "PlatformerPlayer.h"
#include "../Scenes/PlatformerScene.h"
#include "Scenes/HUD_Scene.h"

struct RayCastResultCallback : public b2RayCastCallback
{
	bool m_Hit;
	b2Body* m_pBody;
	RayCastResultCallback()
	{
		m_Hit = false;
		m_pBody = 0;
	}

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		m_Hit = true;
		m_pBody = fixture->GetBody();
		return fraction; //store closest body
	}
};

PlatformerPlayer::PlatformerPlayer(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial)
	: GameObject2D(pScene, name, transform, pMesh, pMaterial)
	, m_pPlayerController(nullptr)
	, m_Speed(PLAYER_SPEED)
	, m_TurningSpeed(PLAYER_SPEED)
{
	m_Score = 0;
}

PlatformerPlayer::~PlatformerPlayer()
{
}

void PlatformerPlayer::Update(float deltatime)
{
	GameObject2D::Update(deltatime);

	b2Vec2 dir(0, 0);
	if (m_GameState == PlayState)
	{
		if (m_pPlayerController)
		{
			if (m_pPlayerController->IsPressed_Jump() && HasJump())
			{
				Jump();
			}

			if (m_pPlayerController->IsPressed_Shoot())
			{
				Shoot();
			}

			if (m_pPlayerController->IsHeld_Left())
			{
				dir.x = -1 * m_Speed;
				m_Direction = ELeft;
			}

			if (m_pPlayerController->IsHeld_Right())
			{
				dir.x = 1 * m_Speed;
				m_Direction = ERight;
			}

			//check for a door
			if (m_pPlayerController->IsPressed_Other())
			{
				CheckForInteractable();
			}

			//rotate the sprite based on input
			if (m_Direction == ELeft)
				m_Rotation.y = 180.0f;
			else
				m_Rotation.y = 0.0f;
		}

		if (m_pBody)
		{
			dir.y = -9.8f; //apply gravity

			float mass = m_pBody->GetMass();
			b2Vec2 currentVel = m_pBody->GetLinearVelocity();
			b2Vec2 VelDiff = dir - currentVel;
			float timestep = 1 / 60.0f;

			b2Vec2 force = mass * VelDiff;
			force.x /= timestep;

			m_pBody->ApplyForceToCenter(force, true);
		}
		else
		{
			vec3 tdir = vec3(dir.x, dir.y, 0.0f);
			m_Position += tdir * m_Speed * deltatime;
		}

		//can always jump while on a wall
		if (m_WallObject)
		{
			JumpCount = 2;
		}
	}
	else
	{
		//stop bosy from sliding away on death/victory
		m_pBody->SetLinearVelocity(b2Vec2(0, 0));
	}

	//Invincibility timer to avoid getting hit twice from the same object. idk why it happened but now it doesn't
	if (m_InvincibilityTimer > 0)
		m_InvincibilityTimer += deltatime;
	if (m_InvincibilityTimer >= 0.1f)
		m_InvincibilityTimer = 0;
}

void PlatformerPlayer::Draw(Camera* cam)
{
	GameObject2D::Draw(cam);
}

void PlatformerPlayer::Jump()
{
	b2Vec2 force;
	force.x = 0.0f;
	force.y = 1.0f;
	force *= JumpHeight;
	m_pBody->ApplyLinearImpulseToCenter(force, true);

	m_pPlayerController->RemoveJumpInput();
	JumpCount--;
}

//spawns bullets and tells them to fire themselves
void PlatformerPlayer::Shoot()
{
	PlatformerScene* scene = (PlatformerScene*)m_pScene;

	GameObject* pObj = scene->m_BulletPool.GetObjectFromPool();

	if (pObj)
	{
		scene->AddGameObject(pObj);

		Bullet* pBullet = (Bullet*)pObj;
		pBullet->Fire(m_Position, m_Direction);
	}

	m_pPlayerController->RemoveShootInput();
}

void PlatformerPlayer::BeginContact(b2Fixture * fix, vec2 pos, vec2 normal)
{
	GameObject* obj = (GameObject*)fix->GetBody()->GetUserData();

	//Enemy collision
	if (m_InvincibilityTimer == 0)
	{
		if (fix->GetFilterData().categoryBits == Collision_Enemy)
		{
			Die();
		}
	}
	//ground collision
	if (normal.x == -1) //the && is to stop ceilings from refreshing jumps
	{
		JumpCount = 2;
	}

	//victory collision
	if (obj->GetName() == "Objective") //Objective is a sensor and signals the end of the game
	{
		PlatformerScene* scene = (PlatformerScene*)m_pScene;
		scene->Victory();
	}
	//Pickup COllision
	else if (fix->GetFilterData().categoryBits == Collision_Pickup) //Pickups
	{
		//set the pickup to no longer interact with other fixtures
		b2Filter filt;
		filt.categoryBits = Collision_Nothing;
		filt.maskBits = fix->GetFilterData().maskBits;

		//turn off the pickups collision
		fix->SetFilterData(filt);

		//increment score and tell the HUD to update
		m_Score += 13;
		m_pScene->GetGame()->GetEventManager()->QueueEvent(new HUD_ValueAdjustEvent(Value_Score, m_Score));

		//hide the pickup texture
		m_pScene->HideGameObject(obj);
	}

	//wall jumping
	//!= are so it doesn't get fired from the floor/ceiling
	std::string stringtype(obj->GetName());
	if (stringtype.find("Ground") != std::string::npos && normal.x != -1 && normal.x != 1)
	{
		m_WallObject = obj;
	}
}

void PlatformerPlayer::EndContact(GameObject2D * pobj)
{
	//if the object we left contact with was our wall, we are now off the wall
	if (pobj == m_WallObject)
	{
		m_WallObject = nullptr;
		JumpCount = 2;
	}
}

//fully reset the player
void PlatformerPlayer::Reset()
{
	GameObject::Reset();

	//Hard reset means putting us back at the start in a fresh run
	if (hardReset)
	{
		m_Score = 0;
		Lives = 2;
		m_GameState = PlayState;

		hardReset = false;
	}

	//HUD_Scene* HUD = ((PlatformerScene*)m_pScene)->GetHUD();

	//pass 2 events to reset the HUD score and life counter
	m_pScene->GetGame()->GetEventManager()->QueueEvent(new HUD_ValueAdjustEvent(Value_Score, m_Score));
	m_pScene->GetGame()->GetEventManager()->QueueEvent(new HUD_ValueAdjustEvent(Value_Life, Lives));
}

//check if on any of the 4 sides there is an interactable object close enough
//used for elevators/doors
void PlatformerPlayer::CheckForInteractable()
{
	RayCastResultCallback raycast;

	int range = 2;

	//UP
	if (Raycast(raycast, b2Vec2(m_Position.x, m_Position.y + range)))
	{
		GameObject2D* obj = (GameObject2D*)raycast.m_pBody->GetUserData();
		
		std::string stringtype(obj->GetName());

		if (stringtype.find("Door") != std::string::npos)
		{
			obj->GetBody()->SetType(b2_dynamicBody);

			b2JointEdge* jointlist = obj->GetBody()->GetJointList();
			b2PrismaticJoint* joint = (b2PrismaticJoint*)jointlist->joint;

			if (joint->IsMotorEnabled())
			{
				joint->EnableMotor(false);
			}
			else
			{
				joint->EnableMotor(true);
			}
		}
	}

	//right
	if (Raycast(raycast, b2Vec2(m_Position.x + range, m_Position.y)))
	{
		GameObject2D* obj = (GameObject2D*)raycast.m_pBody->GetUserData();

		std::string stringtype(obj->GetName());

		if (stringtype.find("Door") != std::string::npos)
		{
			obj->GetBody()->SetType(b2_dynamicBody);

			b2JointEdge* jointlist = obj->GetBody()->GetJointList();
			b2PrismaticJoint* joint = (b2PrismaticJoint*)jointlist->joint;

			if (joint->IsMotorEnabled())
			{
				joint->EnableMotor(false);
			}
			else
			{
				joint->EnableMotor(true);
			}
		}
	}

	//left
	if (Raycast(raycast, b2Vec2(m_Position.x - range, m_Position.y)))
	{
		GameObject2D* obj = (GameObject2D*)raycast.m_pBody->GetUserData();

		std::string stringtype(obj->GetName());

		if (stringtype.find("Door") != std::string::npos)
		{
			obj->GetBody()->SetType(b2_dynamicBody);

			b2JointEdge* jointlist = obj->GetBody()->GetJointList();
			b2PrismaticJoint* joint = (b2PrismaticJoint*)jointlist->joint;

			if (joint->IsMotorEnabled())
			{
				joint->EnableMotor(false);
			}
			else
			{
				joint->EnableMotor(true);
			}
		}
	}

	//down
	if (Raycast(raycast, b2Vec2(m_Position.x, m_Position.y - range)))
	{
		GameObject2D* obj = (GameObject2D*)raycast.m_pBody->GetUserData();

		std::string stringtype(obj->GetName());

		if (stringtype.find("Door") != std::string::npos)
		{
			obj->GetBody()->SetType(b2_dynamicBody);

			b2JointEdge* jointlist = obj->GetBody()->GetJointList();
			b2PrismaticJoint* joint = (b2PrismaticJoint*)jointlist->joint;

			if (joint->IsMotorEnabled())
			{
				joint->EnableMotor(false);
			}
			else
			{
				joint->EnableMotor(true);
			}
		}
	}

	m_pPlayerController->RemoveOtherInput();
}

//actually perform the raycast. Returns success or failure
bool PlatformerPlayer::Raycast(RayCastResultCallback& raycast, b2Vec2 endPoint)
{
	b2Vec2 point1 = b2Vec2(m_Position.x, m_Position.y);
	b2Vec2 point2 = endPoint;

	m_pScene->GetPhysicsWorld()->GetWorld()->RayCast(&raycast, point1, point2);

	return raycast.m_Hit;
}

//Die. If out of lives call gameover
void PlatformerPlayer::Die()
{
	Lives--;

	Lives < 0 ? Lives = 0 : Lives = Lives;

	m_InvincibilityTimer += 0.001f;

	//send a valueadjustevent for life
	m_pScene->GetGame()->GetEventManager()->QueueEvent(new HUD_ValueAdjustEvent(Value_Life, Lives));

	if (Lives == 0)
	{
		PlatformerScene* scene = (PlatformerScene*)m_pScene;
		scene->GameOver();
	}
	else
	{
		m_pScene->m_ShouldReset = true;
	}
}
