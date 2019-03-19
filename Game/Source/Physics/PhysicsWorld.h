#pragma once
class b2World;
class ContactListener;
class DebugDraw;
class Camera;
class Material;

//enum for collision filtering bitmasks used by box2d fixtures
enum CollisionFilterBitMasks
{
	Collision_Nothing = 0x000,		//0x0000
	Collision_Player = 0x0001,        // 0x0001
	Collision_Pickup = 0x0002,        // 0x0002 
	Collision_Enemy = 0x0004,        // 0x0004
	Collision_Environment = 0x0008, // 0x0008
	Collision_Bullet = 0x0010,        // 0x0010
	Collision_Shootable = 0x0020,	//0x0020
};

class PhysicsWorld
{
public:
	PhysicsWorld();
	virtual ~PhysicsWorld();

	void Init(b2Vec2 gravity);

	b2World* GetWorld() { return m_pWorld; }

	void Update(float deltatime);
	void Draw(Camera* cam, Material* mat);

	b2Body* GetWorldBody() { return m_pWorldBody; }

protected:
	b2World* m_pWorld;
	DebugDraw* m_Debugdraw;
	ContactListener* m_pContactListener;
	b2Body* m_pWorldBody;
	float m_TotalTime;

	bool shape = false;
	bool center = false;
	bool joint = false;
	bool aabb = false;
	bool pair = false;
};