#pragma once

class Camera;
class Material;

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
	class DebugDraw3D* debug;

	void Update(float delta);
	void Draw(Camera* cam, Material* mat);

	static void ContactStartedCallback(btPersistentManifold* const& manifold);
	static void ContactEndedCallback(btPersistentManifold* const& manifold);

private:
	bool WireFrame = false;
	bool Constraints = false;
	bool ContactPoints = false;
	bool AABB = false;
	bool Normals = false;
	bool Everything = false;
};
