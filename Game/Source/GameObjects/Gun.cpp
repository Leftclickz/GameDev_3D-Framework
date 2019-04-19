#include "GamePCH.h"

#include "BoshyBullet.h"
#include "../Game/Pool.h"
#include "Gun.h"
#include "Scenes/Scene.h"
#include "Game/ResourceManager.h"

Gun::Gun(Scene * pScene, std::string name)
	: GameObjectComponent(pScene, name)
{
	m_AmmoPool = new Pool<class BoshyBullet*>();

	for (unsigned int i = 0; i < NUM_BULLETS; i++)
	{
		//they add themselves to the pool and delete themselves with the pool
		new BoshyBullet(pScene, "Bullet" + std::to_string(i), Transform(), nullptr, nullptr, m_AmmoPool);
	}
}

Gun::~Gun()
{
	delete m_AmmoPool;

	for (auto i : m_ActiveBullets)
	{
		delete i;
	}
}

void Gun::Shoot()
{
	BoshyBullet* bullet = (BoshyBullet*)m_AmmoPool->GetObjectFromPool();

	m_ActiveBullets.push_back(bullet);

	bullet->SetOwner(this);
	bullet->Fire();
}

void Gun::ReturnToMag(BoshyBullet* bullet)
{
	for (unsigned int i = 0; i < m_ActiveBullets.size(); i++)
	{
		if (m_ActiveBullets[i] == bullet)
		{
			m_AmmoPool->ReturnObjectToPool(bullet);
			m_ActiveBullets.erase(m_ActiveBullets.begin() + i);

			return;
		}
	}
}

void Gun::Update(float delta)
{
	GameObjectComponent::Update(delta);

	for (auto i : m_ActiveBullets)
	{
		i->Update(delta);
	}
}

void Gun::Draw(Camera* cam)
{
	GameObjectComponent::Draw(cam);

	for (auto i : m_ActiveBullets)
	{
		i->Draw(cam);
	}
}

void Gun::Reset()
{
	while (!m_ActiveBullets.empty())
	{
		m_AmmoPool->AddObjectToPool(m_ActiveBullets.back());
		m_ActiveBullets.pop_back();
	}

	m_AmmoPool->Reset();

	GameObjectComponent::Reset();
}
