#pragma once


//----------------------------------------------------------------
class KeyButtonState
{
public:
	KeyButtonState();
	~KeyButtonState();

public:
	bool m_isPressed;
	bool m_wasPressedLastFrame;
};