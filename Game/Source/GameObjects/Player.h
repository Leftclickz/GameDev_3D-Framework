#ifndef __Player_H__
#define __Player_H__

#include "GameObjects/GameObject3D.h"

class Mesh;
class PlayerController;

class Player : public GameObject3D
{
    const float PLAYER_SPEED        = 2.0f;   // Units per second

protected:
    PlayerController* m_pPlayerController;

    float m_Speed;
    float m_TurningSpeed;

public:
	Player(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

    virtual ~Player();

    virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;

    void SetPlayerController(PlayerController* controller) { m_pPlayerController = controller; }
    void SetSpeed(float speed) { m_Speed = speed; }
    void SetTurningSpeed(float speed) { m_TurningSpeed = speed; }

	void Jump();

	virtual void ContactStarted(GameObject3D* pOtherObj) override;
	virtual void ContactEnded(GameObject3D* pOtherObj) override;

	virtual void DisplayImguiDebugInfo() override;
};

#endif //__Player_H__
