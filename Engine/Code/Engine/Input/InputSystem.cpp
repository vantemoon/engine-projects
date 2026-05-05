#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Window/Window.hpp"
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
unsigned char const KEYCODE_UPARROW = VK_UP;
unsigned char const KEYCODE_DOWNARROW = VK_DOWN;
unsigned char const KEYCODE_LEFTARROW = VK_LEFT;
unsigned char const KEYCODE_RIGHTARROW = VK_RIGHT;
unsigned char const KEYCODE_LBUTTON = VK_LBUTTON;
unsigned char const KEYCODE_RBUTTON = VK_RBUTTON;
unsigned char const KEYCODE_TILDE = VK_OEM_3;
unsigned char const KEYCODE_INSERT = VK_INSERT;
unsigned char const KEYCODE_DELETE = VK_DELETE;
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_HOME = VK_HOME;
unsigned char const KEYCODE_END = VK_END;
unsigned char const KEYCODE_SHIFT = VK_SHIFT;
unsigned char const KEYCODE_LBRACKET = VK_OEM_4;
unsigned char const KEYCODE_RBRACKET = VK_OEM_6;


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
	for ( int keyIndex = 0; keyIndex < NUM_KEYCODES; ++keyIndex )
	{
		m_keyStates[keyIndex] = KeyButtonState();
	}
	for ( int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex )
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

	HWND hwnd = static_cast<HWND>( g_engine->m_window->GetHwnd() );
	IntVec2 clientDimensions = g_engine->m_window->GetClientDimensions();

	if ( m_cursorState.m_mode == CursorMode::FPS )
	{
		while ( ShowCursor( FALSE ) >= 0 )
		{
			// Loop until Windows reports the cursor is hidden (negative return value)
		}
	}
	else
	{
		while ( ShowCursor( TRUE ) < 0 )
		{
			// Loop until Windows reports the cursor is shown (non-negative return value)
		}
	}

	bool isWindowFocused = g_engine->m_window->IsFocused();
	IntVec2 previousCursorClientPosition = m_cursorState.m_cursorClientPosition;

	POINT cursorScreenPos;
	GetCursorPos( &cursorScreenPos );
	ScreenToClient( hwnd, &cursorScreenPos );
	IntVec2 currentCursorClientPosition = IntVec2( cursorScreenPos.x, cursorScreenPos.y );

	if ( m_cursorState.m_mode == CursorMode::FPS )
	{
		if ( isWindowFocused )
		{
			m_cursorState.m_cursorClientDelta = currentCursorClientPosition - previousCursorClientPosition;
		}
		else
		{
			m_cursorState.m_cursorClientDelta = IntVec2( 0, 0 );
		}

		IntVec2 centerClient = IntVec2( clientDimensions.x / 2, clientDimensions.y / 2 );
		POINT centerScreen = { centerClient.x, centerClient.y };
		ClientToScreen( hwnd, &centerScreen );
		SetCursorPos( centerScreen.x, centerScreen.y );

		POINT recenteredScreenPos;
		GetCursorPos( &recenteredScreenPos );
		ScreenToClient( hwnd, &recenteredScreenPos );
		m_cursorState.m_cursorClientPosition = IntVec2( recenteredScreenPos.x, recenteredScreenPos.y );
	}
	else
	{
		m_cursorState.m_cursorClientDelta    = IntVec2( 0, 0 );
		m_cursorState.m_cursorClientPosition = currentCursorClientPosition;
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


//----------------------------------------------------------------
void InputSystem::SetCursorMode( CursorMode mode )
{
	m_cursorState.m_mode = mode;
}


//----------------------------------------------------------------
IntVec2 InputSystem::GetCursorClientDelta() const
{
	return m_cursorState.m_cursorClientDelta;
}


//----------------------------------------------------------------
IntVec2 InputSystem::GetCursorClientPosition() const
{
	return m_cursorState.m_cursorClientPosition;
}


//----------------------------------------------------------------
IntVec2 InputSystem::GetCursorNormalizedPosition() const
{
	IntVec2 clientDimensions = g_engine->m_window->GetClientDimensions();
	IntVec2 cursorPosition = GetCursorClientPosition();
	IntVec2 normalizedPosition = IntVec2( cursorPosition.x * 1000 / clientDimensions.x, cursorPosition.y * 1000 / clientDimensions.y );
	return normalizedPosition;
}


//----------------------------------------------------------------
bool InputSystem::Event_KeyPressed( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_inputSystem )
	{
		unsigned char keyCode = static_cast<unsigned char>( args.GetValue( "KeyCode", -1 ) );
		if ( keyCode >= 0 && keyCode < NUM_KEYCODES )
		{
			g_engine->m_inputSystem->HandleKeyPressed( keyCode );
			return true; // Consumes event; do not call other subscribers' callback functions
		}
	}
	return false; // Does not consume event; continue to call other subscribers' callback functions
}


//----------------------------------------------------------------
bool InputSystem::Event_KeyReleased( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_inputSystem )
	{
		unsigned char keyCode = static_cast<unsigned char>( args.GetValue( "KeyCode", -1 ) );
		if ( keyCode >= 0 && keyCode < NUM_KEYCODES )
		{
			g_engine->m_inputSystem->HandleKeyReleased( keyCode );
			return true; // Consumes event; do not call other subscribers' callback functions
		}
	}
	return false; // Does not consume event; continue to call other subscribers' callback functions
}


//----------------------------------------------------------------
bool InputSystem::Event_CharacterInput( EventArgs& args )
{
	UNUSED( args );
	if ( g_engine && g_engine->m_inputSystem )
	{
		std::string characterText = args.GetValue( "Character", "" );
		char character;
		if ( characterText.size() > 0 )
		{
			character = characterText[0];
		}
		else
		{
			character = '\0';
		}
		UNUSED( character );
		// Handle character input if needed (e.g. for text input fields, etc.)
		return true; // Consumes event; do not call other subscribers' callback functions
	}
	return false; // Does not consume event; continue to call other subscribers' callback functions
}