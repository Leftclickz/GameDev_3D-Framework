#ifndef __PlatformerPlayer_H__
#define __PlatformerPlayer_H__

class Mesh;
class PlayerController;
#include "GameObjects/GameObject2D.h"

enum EGameState
{
	PlayState,
	WinState,
	LoseState

};

class PlatformerPlayer : public GameObject2D
{
	const float PLAYER_SPEED = 4.5f;   // Units per second

protected:
	PlayerController* m_pPlayerController;

	float m_Speed;
	float m_TurningSpeed;

	float JumpHeight = 10.0f;
	char JumpCount = 2;
	short Lives = 2;

	unsigned int m_Score;

	GameObject* m_WallObject = nullptr;

	vec3 originalpos;

	EDirection m_Direction = ERight;

	float m_InvincibilityTimer = 0.0f;

public:
	bool hardReset = false;

	PlatformerPlayer(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~PlatformerPlayer();

	virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;

	void SetPlayerController(PlayerController* controller) { m_pPlayerController = controller; }
	void SetSpeed(float speed) { m_Speed = speed; }
	void SetTurningSpeed(float speed) { m_TurningSpeed = speed; }

	bool HasJump() { return JumpCount > 0 ? true : false; }
	void Jump();

	void Shoot();

	virtual void BeginContact(b2Fixture* fix, vec2 pos, vec2 normal) override;
	virtual void EndContact(GameObject2D* pobj) override;

	virtual void Reset() override;
	virtual void CheckForInteractable();
	bool Raycast(struct RayCastResultCallback& raycast, b2Vec2 endPoint);

	void Die();

	EGameState m_GameState = PlayState;
};

#endif //__PlatformerPlayer_H__
