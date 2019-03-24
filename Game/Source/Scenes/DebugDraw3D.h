#pragma once
#include "../Framework/Libraries/Bullet3/src/LinearMath/btIDebugDraw.h"

class Camera;
class Material;

class DebugDraw3D : public btIDebugDraw
{
	int m_debugMode;

public:

	DebugDraw3D();

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

	virtual void reportErrorWarning(const char* warningString) override;

	virtual void draw3dText(const btVector3& location, const char* textString) override;

	virtual void setDebugMode(int debugMode) override;

	virtual int getDebugMode() const override;

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	virtual void SetCamera(Camera* cam) { m_pCamera = cam; }
	virtual void SetMaterial(Material* mat) { m_pMaterial = mat; }

private:
	Camera* m_pCamera;
	Material* m_pMaterial;
};