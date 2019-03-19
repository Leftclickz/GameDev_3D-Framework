#pragma once

class GameObject3D;

class BulletMotionState : public btMotionState
{
public:
	BulletMotionState(GameObject3D* pObj);
	~BulletMotionState();

	virtual void getWorldTransform(btTransform& worldTrans) const override;
	virtual void setWorldTransform(const btTransform& worldTrans) override;

protected:
	GameObject3D* m_pGamebject;
};