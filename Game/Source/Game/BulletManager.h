#pragma once

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	void Update(float delta);

	static void ContactStartedCallback(btPersistentManifold* const& manifold);
	static void ContactEndedCallback(btPersistentManifold* const& manifold);
};
