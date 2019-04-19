#ifndef __Player_H__
#define __Player_H__

#include "GameObjects/GameObject3D.h"

class Mesh;
class PlayerController;

class Player : public GameObject3D
{
    const float PLAYER_SPEED = 10.0f;   // Units per second
    const float PLAYER_HEALTH = 100.0f;   
	const char MAX_JUMPS = 2;
	const float PLAYER_JUMP_HEIGHT = 20.0f;
	const vec3 FOLLOW_LIGHT_OFFSET = vec3(0, 5, 5);

protected:
    PlayerController* m_pPlayerController;

    float m_Speed;
	float m_Health;

	float m_JumpHeight = PLAYER_JUMP_HEIGHT;
	unsigned char m_JumpCount = MAX_JUMPS;

	class FollowLight* m_Followlight;
	std::map<std::string, class AudioList*> m_PlayerSounds;

	class Gun* m_Gun;

	vec3 m_RespawnPoint;
	vec3 m_RespawnRot;

public:
	Player(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

    virtual ~Player();

    virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;

    void SetPlayerController(PlayerController* controller) { m_pPlayerController = controller; }
    void SetSpeed(float speed) { m_Speed = speed; }

	void SetRespawnData(vec3 pos, vec3 rot) { m_RespawnPoint = pos; m_RespawnRot = rot; }

	void Jump();

	virtual void ContactStarted(GameObject3D* pOtherObj, vec3 normal) override;
	virtual void ContactEnded(GameObject3D* pOtherObj) override;

	virtual void DisplayImguiDebugInfo() override;

	virtual void TakeDamage(float amount);

	virtual void Die();
	virtual void Reset() override;
};

#endif //__Player_H__
