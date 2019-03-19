#ifndef __PlatformerEnemy_H__
#define __PlatformerEnemy_H__

class Mesh;
class PlayerController;

class PlatformerEnemy : public GameObject2D
{
public:
	PlatformerEnemy(Scene* pScene, std::string name, Transform transform, Mesh* pMesh, Material* pMaterial);

	virtual ~PlatformerEnemy();

	virtual void Update(float deltatime) override;
	virtual void Draw(Camera* cam) override;
};

#endif //__PlatformerEnemy_H__
