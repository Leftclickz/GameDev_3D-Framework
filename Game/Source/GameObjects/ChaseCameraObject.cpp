#include "GamePCH.h"

#include "ChaseCameraObject.h"
#include "GameObjects/Camera.h"
#include "Game/Game.h"
#include "../Scenes/Scene.h"

ChaseCameraObject::ChaseCameraObject(Scene* pScene, std::string name, Transform transform)
: Camera( pScene, name, transform)
{
    m_ObjectFollowing = nullptr;
    m_Distance = 0;

    m_MouseLocked = true;
    m_MouseChangeSinceLastFrame.Set( 0, 0 );
    m_LastMousePosition.Set( 0, 0 );
	m_Rotation.x -= 30.0f;

	m_RespawnRotation = vec3(-30.0f, 0.0, 0.0);
}

ChaseCameraObject::~ChaseCameraObject()
{
}

void ChaseCameraObject::OnEvent(Event* pEvent)
{
    if( pEvent->GetEventType() == EventType_Input )
    {
#if WIN32
        InputEvent* pInput = static_cast<InputEvent*>( pEvent );

        if( pInput->GetInputDeviceType() == InputDeviceType_Keyboard )
        {
            int keyID = pInput->GetID();

            if( pInput->GetInputState() == InputState_Pressed )
            {
                if( keyID == 'C' )
                {
                    if( m_MouseLocked )
                    {
                        m_MouseLocked = false;
                        m_MouseChangeSinceLastFrame.Set( 0, 0 );
                        m_LastMousePosition.Set(640, 360);
                    }
                    else
                    {
                        m_MouseLocked = true;
                        m_LastMousePosition.Set( 640, 360 );
                        SetCursorPos( (int)m_LastMousePosition.x, (int)m_LastMousePosition.y );
                    }
                }
            }
        }

        if( pInput->GetInputDeviceType() == InputDeviceType_Mouse )
        {
            if( m_MouseLocked )
            {
                m_MouseLocked = true;
                m_MouseChangeSinceLastFrame = m_LastMousePosition - pInput->GetPosition();
				m_Rotation += vec3(m_MouseChangeSinceLastFrame.y * 0.5f, m_MouseChangeSinceLastFrame.x * 0.5f, 0.0f);

                m_LastMousePosition.Set(640, 360 );
                m_pScene->GetGame()->GetFramework()->SetMousePosition( ivec2( (int)m_LastMousePosition.x, (int)m_LastMousePosition.y ) );
            }
        }
#endif
    }
}

void ChaseCameraObject::Update(float deltaTime)
{
    Camera::Update( deltaTime );

    if( m_ObjectFollowing == 0 )
    {
        m_ViewMatrix.CreateSRT( m_Scale, m_Rotation, m_Position );
        m_ViewMatrix.Inverse();
    }
    else
    {
		if (m_pScene->GetGame()->GetFramework()->IsKeyDown(VK_UP))
		{
			float radianangle = (m_Rotation.x - 1.0f) / 180 * PI;
			vec3 newpos = vec3(cosf(radianangle) * m_Position.x, m_Position.y, sinf(radianangle) * m_Position.z);
			SetPosition(newpos);
			m_Rotation.x = radianangle / PI * 180;
		}
		else if (m_pScene->GetGame()->GetFramework()->IsKeyDown(VK_RIGHT))
		{
			float radianangle = (m_Rotation.y - 1.0f) / 180 * PI;
			vec3 newpos = vec3(cosf(radianangle) * m_Position.x, m_Position.y, sinf(radianangle) * m_Position.z);

			SetPosition(newpos);
			m_Rotation.y = radianangle / PI * 180;
		}
		else if (m_pScene->GetGame()->GetFramework()->IsKeyDown(VK_DOWN))
		{
			float radianangle = (m_Rotation.x + 1.0f) / 180 * PI;
			vec3 newpos = vec3(cosf(radianangle) * m_Position.x, m_Position.y, sinf(radianangle) * m_Position.z);

			SetPosition(newpos);
			m_Rotation.x = radianangle / PI * 180;
		}
		else if (m_pScene->GetGame()->GetFramework()->IsKeyDown(VK_LEFT))
		{
			float radianangle = (m_Rotation.y + 1.0f) / 180 * PI;
			vec3 newpos = vec3(cosf(radianangle) * m_Position.x, m_Position.y, sinf(radianangle) * m_Position.z);

			SetPosition(newpos);
			m_Rotation.y = radianangle / PI * 180;
		}

		if (m_Rotation.y > 360)
			m_Rotation.y = 0;

        if( m_pScene->GetGame()->GetFramework()->IsKeyDown( 'U' ) )
            m_Distance -= 10 * deltaTime;
        if( m_pScene->GetGame()->GetFramework()->IsKeyDown( 'M' ) )
            m_Distance += 10 * deltaTime;

        MyClamp( m_Distance, 1.0f, 20.0f );
        MyClamp( m_Rotation.x, -89.9f, 89.9f );

        vec3 followedObjectPosition = m_ObjectFollowing->GetPosition() + vec3( 0, 1, 0 );

		m_ViewMatrix.SetIdentity();
		m_ViewMatrix.Translate(0, 0, -m_Distance);
		m_ViewMatrix.Rotate(m_Rotation.x, 1, 0, 0);
		m_ViewMatrix.Rotate(m_Rotation.y, 0, 1, 0);
		m_ViewMatrix.Translate(followedObjectPosition);
		m_ViewMatrix.Inverse();

		float sx = sin(m_Rotation.x / 180 * PI);
		float sy = sin(m_Rotation.y / 180 * PI);
		float cx = cos(m_Rotation.x / 180 * PI);
		float cy = cos(m_Rotation.y / 180 * PI);

		m_Position = followedObjectPosition + vec3(cx * sy * m_Distance, -sx * m_Distance, -1 * cx * cy * m_Distance);

		m_ViewMatrix.CreateLookAtView(m_Position, vec3(0, 1, 0), followedObjectPosition);
    }
}

void ChaseCameraObject::Reset()
{
	m_Rotation = m_RespawnRotation;
}
