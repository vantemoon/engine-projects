#include "Engine/Input/KeyButtonState.hpp"


//----------------------------------------------------------------
KeyButtonState::KeyButtonState()
	: m_isPressed( false )
	, m_wasPressedLastFrame( false )
{
}


//----------------------------------------------------------------
KeyButtonState::~KeyButtonState()
{
	// DO NOTHING
}