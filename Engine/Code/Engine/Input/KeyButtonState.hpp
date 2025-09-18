#pragma once


//----------------------------------------------------------------
class KeyButtonState
{
public:
	KeyButtonState() = default;
	~KeyButtonState() = default;

public:
	bool m_isPressed = false;
	bool m_wasPressedLastFrame = false;
};