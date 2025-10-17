#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/AnalogJoystick.hpp"
#include <Windows.h>


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
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_UPARROW = VK_UP;
unsigned char const KEYCODE_DOWNARROW = VK_DOWN;
unsigned char const KEYCODE_LEFTARROW = VK_LEFT;
unsigned char const KEYCODE_RIGHTARROW = VK_RIGHT;


//-----------------------------------------------------------------------------------------------
InputSystem::InputSystem( InputConfig const& config )
	: m_config( config )
{
}


//----------------------------------------------------------------
void InputSystem::StartUp()
{
	for ( int keyIndex = 0; keyIndex < NUM_KEYCODES; ++keyIndex )
	{
		m_keyStates[keyIndex] = KeyButtonState();
	}
	for ( int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex )
	{
		m_controllers[controllerIndex] = XboxController( controllerIndex );
	}
}


//----------------------------------------------------------------
void InputSystem::ShutDown()
{
    for (int keyIndex = 0; keyIndex < NUM_KEYCODES; ++keyIndex)
    {
        m_keyStates[keyIndex] = KeyButtonState();
    }
    for (int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex)
    {
        m_controllers[controllerIndex].Reset();
    }
}


//----------------------------------------------------------------
void InputSystem::BeginFrame()
{
	for ( int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex )
	{
		m_controllers[controllerIndex].Update();
	}
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


//----------------------------------------------------------------
XboxController const& InputSystem::GetController( int controllerID )
{
	if ( controllerID < 0 || controllerID >= NUM_XBOX_CONTROLLERS )
	{
		ERROR_AND_DIE( "InputSystem::GetController() - controllerID is out of range!" );
	}
	return m_controllers[controllerID];
}