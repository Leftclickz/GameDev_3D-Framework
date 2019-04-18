#include "GamePCH.h"

#include "GameObjects/PlayerController.h"

void PlayerController::OnEvent(Event* pEvent)
{
    if( pEvent->GetEventType() == EventType_Input )
    {
#if WIN32
        InputEvent* pInput = (InputEvent*)pEvent;

        if( pInput->GetInputDeviceType() == InputDeviceType_Keyboard )
        {
            int keyID = pInput->GetID();

            if( pInput->GetInputState() == InputState_Pressed )
            {
                if( keyID == 'W' ) { m_Directions |= DIR_UP; }
                if( keyID == 'S' ) { m_Directions |= DIR_DOWN; }
                if( keyID == 'A' ) { m_Directions |= DIR_LEFT; }
                if( keyID == 'D' ) { m_Directions |= DIR_RIGHT; }

                if( keyID == 'Q' ) { m_Directions |= DIR_OUT; }
                if( keyID == 'E' ) { m_Directions |= DIR_IN; }

				if (keyID == VK_UP) { m_Directions |= DIR_AIMUP; }
				if (keyID == VK_DOWN) { m_Directions |= DIR_AIMDOWN; }
				if (keyID == VK_LEFT) { m_Directions |= DIR_AIMLEFT; }
				if (keyID == VK_RIGHT) { m_Directions |= DIR_AIMRIGHT; }

				if (keyID == 'C' && !isOther) { m_Directions |= DIR_OTHER; isOther = true; }
				if (keyID == ' ' && !isJumping) { m_Directions |= DIR_JUMP; isJumping = true; }
				if (keyID == 'X' && !isShooting) { m_Directions |= DIR_SHOOT; isShooting = true; }
            }

            if( pInput->GetInputState() == InputState_Released )
            {
                if( keyID == 'W' ) { m_Directions &= ~DIR_UP; }
                if( keyID == 'S' ) { m_Directions &= ~DIR_DOWN; }
                if( keyID == 'A' ) { m_Directions &= ~DIR_LEFT; }
                if( keyID == 'D' ) { m_Directions &= ~DIR_RIGHT; }

				if (keyID == 'Q') { m_Directions &= ~DIR_OUT; }
				if (keyID == 'E') { m_Directions &= ~DIR_IN; }

				if (keyID == VK_UP) { m_Directions &= ~DIR_AIMUP; }
				if (keyID == VK_DOWN) { m_Directions &= ~DIR_AIMDOWN; }
				if (keyID == VK_LEFT) { m_Directions &= ~DIR_AIMLEFT; }
				if (keyID == VK_RIGHT) { m_Directions &= ~DIR_AIMRIGHT; }

				if (keyID == 'C') { m_Directions &= ~DIR_OTHER; isOther = false; }
				if (keyID == ' ') { m_Directions &= ~DIR_JUMP; isJumping = false; }
				if (keyID == 'X') { m_Directions &= ~DIR_SHOOT; isShooting = false; }
            }
        }
		if (pInput->GetInputDeviceType() == InputDeviceType_Mouse)
		{
			int keyID = pInput->GetID();

			if (pInput->GetInputState() == InputState_Pressed)
			{
				if (keyID == 0 && !isShooting) { m_Directions |= DIR_SHOOT; isShooting = true; }
			}

			if (pInput->GetInputState() == InputState_Released)
			{
				if (keyID == 0) { m_Directions &= ~DIR_SHOOT; isShooting = false; }
			}
		}
#endif
    }
}
