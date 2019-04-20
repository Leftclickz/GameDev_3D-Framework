#include "GamePCH.h"
#include "BulletManager.h"
#include "GameObjects/GameObject3D.h"
#include "Scenes/DebugDraw3D.h"

#undef new //defined as new(__FILE__, __LINE__) by MyMemory.h

BulletManager::BulletManager()
{
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -30, 0));

	gContactStartedCallback = ContactStartedCallback;
	gContactEndedCallback = ContactEndedCallback;

	debug = new DebugDraw3D();
	dynamicsWorld->setDebugDrawer(debug);

	///-----initialization_end-----
}

BulletManager::~BulletManager()
{
	delete debug;
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}

void BulletManager::Update(float delta)
{
	///-----stepsimulation_start-----
	dynamicsWorld->stepSimulation(1.f / 60.f, 10);

	//print positions of all objects
	for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = obj->getWorldTransform();
		}
		//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
	}

	///-----stepsimulation_end-----
}

void BulletManager::Draw(Camera* cam, Material* mat)
{
	//set flags here
	int debugmode = DebugDraw3D::DBG_NoDebug;

	if (WireFrame)
		debugmode |= DebugDraw3D::DBG_DrawWireframe;

	if (Constraints)
		debugmode |= DebugDraw3D::DBG_DrawConstraints;

	if (ContactPoints)
		debugmode |= DebugDraw3D::DBG_DrawContactPoints;

	if (AABB)
		debugmode |= DebugDraw3D::DBG_DrawAabb;

	if (Normals)
		debugmode |= DebugDraw3D::DBG_DrawNormals;

	if (Everything)
		debugmode |= DebugDraw3D::DBG_MAX_DEBUG_DRAW_MODE;


	debug->setDebugMode(debugmode);

	debug->SetCamera(cam);
	debug->SetMaterial(mat);

	dynamicsWorld->debugDrawWorld();
}

void BulletManager::ContactStartedCallback(btPersistentManifold* const& manifold)
{
	GameObject3D* pObjA =  (GameObject3D*)manifold->getBody0()->getUserPointer();
	GameObject3D* pObjB =  (GameObject3D*)manifold->getBody1()->getUserPointer();

	btVector3 avel = pObjA->GetBody()->getLinearVelocity();
	btVector3 bvel = pObjB->GetBody()->getLinearVelocity();

	vec3 normal = vec3(0);

	int numContacts = manifold->getNumContacts();
	for (int j = 0; j < numContacts; j++)
	{
		btManifoldPoint& pt = manifold->getContactPoint(j);

		btVector3 ptA = pt.getPositionWorldOnA();
		btVector3 ptB = pt.getPositionWorldOnB();

		btVector3 dir = ptA - ptB;

		normal += vec3(dir.getX(), dir.getY(), dir.getZ());
	}
	if(normal != vec3(0))
		normal.Normalize();

	if (avel.getY() < 0 && normal.y >0)
	{
		normal.y *= -1;
		pObjA->ContactStarted(pObjB, normal);
	}
	else
	{
		pObjA->ContactStarted(pObjB, normal);
	}

	if (bvel.getY() < 0 && normal.y > 0)
	{
		normal.y *= -1;
		pObjB->ContactStarted(pObjA, normal);
	}
	else
	{
		pObjB->ContactStarted(pObjA, normal);
	}
}

void BulletManager::ContactEndedCallback(btPersistentManifold* const& manifold)
{
	GameObject3D* pObjA = (GameObject3D*)manifold->getBody0()->getUserPointer();
	GameObject3D* pObjB = (GameObject3D*)manifold->getBody1()->getUserPointer();

	pObjA->ContactEnded(pObjB);
	pObjB->ContactEnded(pObjA);
}

void BulletManager::ImGuiDisplayDebugData()
{
	ImGui::Begin("Debug Drawing");
	ImGui::Checkbox("Debug Draw Everything", &Everything);
	ImGui::Checkbox("Debug Draw Wireframe", &WireFrame);
	ImGui::Checkbox("Debug Draw ContactPoints", &ContactPoints);
	ImGui::Checkbox("Debug Draw AABB", &AABB);
	ImGui::Checkbox("Debug Draw Constraints", &Constraints);
	ImGui::Checkbox("Debug Draw Normals", &Normals);
	ImGui::End();
}

btCollisionWorld::ClosestRayResultCallback BulletManager::PerformRaycast(vec3 start, vec3 end)
{
	btVector3 StartVec = btVector3(start.x, start.y, start.z);
	btVector3 EndVec = btVector3(end.x, end.y, end.z);

	btCollisionWorld::ClosestRayResultCallback RayCast(StartVec, EndVec);

	dynamicsWorld->rayTest(StartVec, EndVec, RayCast);

	return RayCast;
}
