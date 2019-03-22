#include "GamePCH.h"
#include "Camera.h"

Camera::Camera(Scene* pScene, std::string name, Transform transform, vec3 lookat, float range)
	: GameObject(pScene, name, transform, nullptr, nullptr)
{
	m_LookAtPosition = lookat;
	m_Range = range;
	m_Offset = vec3(0);
	m_LERPCap = 0.0f;
	m_Target = nullptr;
}

Camera::~Camera()
{
}

void Camera::CreateViewMatrix(vec3 up /*= vec3(0,1,0)*/)
{
	m_ViewMatrix.CreateLookAtView(m_Position, up, m_LookAtPosition);
}

void Camera::CreateProjectionMatrix(float FOV /*= 45.0f*/, float nearZ /*= 0.01f*/, float aspect /*= 1.0f */)
{
	//if the camera is set to be orthographic use that instead and set it to the screen's size (HARDCODED)
	if (!m_isOrtho)
	{
		m_ProjectionMatrix.CreatePerspectiveVFoV(FOV, aspect, nearZ, m_Range);
	}
	else
	{
		m_ProjectionMatrix.CreateOrtho(0.0f, 600.0f, 0.0f, 600.0f, -0.01f, m_Range);
	}
}

void Camera::SetLookAtPosition(vec3 lookatpos)
{
	m_LookAtPosition = lookatpos;

	if (m_Target)
	{
		m_Target = nullptr;
	}
}

void Camera::SetLookAtTarget(GameObject* pObj, vec3 offset)
{
	m_LookAtPosition = pObj->GetPosition();
	m_Target = pObj;
	m_Offset = offset;
}

void Camera::LERPToTarget(float LERPTimer, GameObject* pObj, vec3 offset)
{
	m_LERPTimer = 0.0f;
	m_LERPCap = LERPTimer;
	m_LERPTarget = pObj;
	m_LERPOffset = m_Position - offset;
	m_LERPFinalOffset = offset;

}

void Camera::Update(float deltatime)
{
	//lerping is priority over target following
	if (m_LERPCap > 0.0f)
	{
		//update timer
		m_LERPTimer += deltatime;
		m_LERPTimer = m_LERPTimer > m_LERPCap ? m_LERPCap : m_LERPTimer;

		//percent of delta on timer
		float deltaPercent = deltatime / m_LERPCap;

		//delta values of our lookat, position and offset
		vec3 LookAtDelta = m_LERPTarget->GetPosition() * deltaPercent;
		vec3 PositionDelta = m_LERPTarget->GetPosition() * deltaPercent;
		vec3 OffsetDelta = (m_Offset - m_LERPOffset) * deltaPercent;

		//add the deltas
		SetLookAtPosition(GetLookAtPosition() + LookAtDelta);
		SetPosition(GetPosition() + PositionDelta + OffsetDelta);

		//once we complete the timer, set the look target to our lerped one and reset lerp values
		if (m_LERPTimer == m_LERPCap)
		{
			SetLookAtTarget(m_LERPTarget, m_LERPFinalOffset);

			//reset LERP values
			m_LERPTarget = nullptr;
			m_LERPOffset = vec3(0);
			m_LERPFinalOffset = vec3(0);
			m_LERPCap = 0.0f;
			m_LERPTimer = 0.0f;
		}
	}
	//follow a target if we have one and arent lerping.
	else 
	if (m_Target)
	{
		m_LookAtPosition = m_Target->GetPosition();
		m_Position = m_Target->GetPosition();
		m_Position += m_Offset;
	}

	//create matrices
	CreateViewMatrix();
	CreateProjectionMatrix();
}
