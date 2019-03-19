#include "GamePCH.h"
#include "BulletMotionState.h"
#include "../GameObjects/GameObject3D.h"

BulletMotionState::BulletMotionState(GameObject3D* pObj)
{
	m_pGamebject = pObj;
}

BulletMotionState::~BulletMotionState()
{
}

void BulletMotionState::getWorldTransform(btTransform& worldTrans) const
{
	MyMatrix matrix;
	matrix.CreateSRT(vec3(1), m_pGamebject->GetRotation(), m_pGamebject->GetPosition());
	worldTrans.setFromOpenGLMatrix(&matrix.m11);
}

void BulletMotionState::setWorldTransform(const btTransform& worldTrans)
{
	btTransform trans = m_pGamebject->m_Body->getWorldTransform();

	MyMatrix matrix;
	worldTrans.getOpenGLMatrix(&matrix.m11);

	m_pGamebject->m_Position = matrix.GetTranslation();
	m_pGamebject->m_Rotation = matrix.GetEulerAngles() * 180.0f / PI;

}
