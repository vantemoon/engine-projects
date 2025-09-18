#include <Windows.h>
#include "Engine/Input/InputSystem.hpp"


//----------------------------------------------------------------
unsigned char const KEYCODE_F1 = VK_F1;
unsigned char const KEYCODE_F2 = VK_F2;
unsigned char const KEYCODE_F3 = VK_F3;
unsigned char const KEYCODE_F4 = VK_F4;
unsigned char const KEYCODE_F5 = VK_F5;
unsigned char const KEYCODE_F6 = VK_F6;
unsigned char const KEYCODE_F7 = VK_F7;
unsigned char const KEYCODE_F8 = VK_F8;
unsigned char const KEYCODE_F9 = VK_F9;
unsigned char const KEYCODE_F10 = VK_F10;
unsigned char const KEYCODE_F11 = VK_F11;
unsigned char const KEYCODE_ESCAPE = VK_ESCAPE;
unsigned char const KEYCODE_SPACE = VK_SPACE;
unsigned char const KEYCODE_ENTER = VK_RETURN;


//----------------------------------------------------------------
InputSystem::InputSystem()
{
}


//----------------------------------------------------------------
InputSystem::~InputSystem()
{
}


//----------------------------------------------------------------
void InputSystem::StartUp()
{
}


//----------------------------------------------------------------
void InputSystem::ShutDown()
{
}


//----------------------------------------------------------------
void InputSystem::BeginFrame()
{
}


//----------------------------------------------------------------
void InputSystem::EndFrame()
{
	for ( int keyIndex = 0; keyIndex < NUM_KEYCODES; ++keyIndex )
	{
		m_keyStates[keyIndex].m_wasPressedLastFrame = m_keyStates[keyIndex].m_isPressed;
	}
}


//----------------------------------------------------------------
bool InputSystem::IsKeyDown( unsigned char keyCode ) const
{
	return m_keyStates[keyCode].m_isPressed;
}


//----------------------------------------------------------------
bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
	return ( m_keyStates[keyCode].m_isPressed && !m_keyStates[keyCode].m_wasPressedLastFrame );
}


//----------------------------------------------------------------
bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
	return ( !m_keyStates[keyCode].m_isPressed && m_keyStates[keyCode].m_wasPressedLastFrame );
}


//----------------------------------------------------------------
void InputSystem::HandleKeyPressed( unsigned char keyCode )
{
	m_keyStates[keyCode].m_isPressed = true;
}


//----------------------------------------------------------------
void InputSystem::HandleKeyReleased( unsigned char keyCode )
{
	m_keyStates[keyCode].m_isPressed = false;
}