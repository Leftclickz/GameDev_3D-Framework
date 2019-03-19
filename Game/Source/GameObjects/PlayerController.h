#ifndef __PlayerController_H__
#define __PlayerController_H__

class Event;

class PlayerController
{
    enum Directions
    {
        DIR_UP      = 0x0001,
        DIR_DOWN    = 0x0002,
        DIR_LEFT    = 0x0004,
        DIR_RIGHT   = 0x0008,
		DIR_IN		= 0x0010,
		DIR_OUT		= 0x0020,
		DIR_JUMP	= 0x0040,
		DIR_OTHER	= 0x0080,
		DIR_SHOOT	= 0x0100,
    };

protected:
    int m_Directions;
	bool isJumping = false;
	bool isShooting = false;
	bool isOther = false;

public:
    void OnEvent(Event* pEvent);

    bool IsHeld_Up() { return m_Directions & DIR_UP; }
    bool IsHeld_Down() { return m_Directions & DIR_DOWN; }
    bool IsHeld_Left() { return m_Directions & DIR_LEFT; }
    bool IsHeld_Right() { return m_Directions & DIR_RIGHT; }
    bool IsHeld_In() { return m_Directions & DIR_IN; }
    bool IsHeld_Out() { return m_Directions & DIR_OUT; }

    bool IsPressed_Jump() { return m_Directions & DIR_JUMP; }
    bool IsPressed_Shoot() { return m_Directions & DIR_SHOOT; }

    bool IsPressed_Other() { return m_Directions & DIR_OTHER; }

	void RemoveJumpInput() { m_Directions &= ~DIR_JUMP; }
	void RemoveShootInput() { m_Directions &= ~DIR_SHOOT; }
	void RemoveOtherInput() { m_Directions &= ~DIR_OTHER; }
};

#endif //__PlayerController_H__
