#pragma once
#include "GameObjectComponent.h"

class Gun : public GameObjectComponent
{
	const unsigned int NUM_BULLETS = 100;

public:
	Gun(Scene * pScene, std::string name);
	virtual ~Gun();

	void Shoot();
	void ReturnToMag(class BoshyBullet* bullet);

	virtual void Update(float delta) override;
	virtual void Draw(Camera* cam) override;

	virtual void Reset() override;

protected:
	class Pool<class BoshyBullet*>* m_AmmoPool;
	std::vector<BoshyBullet*> m_ActiveBullets;
};