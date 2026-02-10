#pragma once
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EventSystem.hpp"


//----------------------------------------------------------------
extern unsigned char const KEYCODE_F1;
extern unsigned char const KEYCODE_F2;
extern unsigned char const KEYCODE_F3;
extern unsigned char const KEYCODE_F4;
extern unsigned char const KEYCODE_F5;
extern unsigned char const KEYCODE_F6;
extern unsigned char const KEYCODE_F7;
extern unsigned char const KEYCODE_F8;
extern unsigned char const KEYCODE_F9;
extern unsigned char const KEYCODE_F10;
extern unsigned char const KEYCODE_F11;
extern unsigned char const KEYCODE_ESCAPE;
extern unsigned char const KEYCODE_SPACE;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const KEYCODE_LEFTARROW;
extern unsigned char const KEYCODE_RIGHTARROW;
extern unsigned char const KEYCODE_UPARROW;
extern unsigned char const KEYCODE_DOWNARROW;
extern unsigned char const KEYCODE_LBUTTON;
extern unsigned char const KEYCODE_RBUTTON;
extern unsigned char const KEYCODE_TILDE;
extern unsigned char const KEYCODE_INSERT;
extern unsigned char const KEYCODE_DELETE;
extern unsigned char const KEYCODE_HOME;
extern unsigned char const KEYCODE_END;


//----------------------------------------------------------------
constexpr int NUM_KEYCODES = 256;
constexpr int NUM_XBOX_CONTROLLERS = 4;


//-----------------------------------------------------------------------------------------------
struct InputConfig
{
	bool m_isEnabled = true;
};


//----------------------------------------------------------------
class InputSystem
{
public:
	InputSystem( InputConfig const& config );
	~InputSystem() = default;
	void StartUp();
	void ShutDown();
	void BeginFrame();
	void EndFrame();

	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

	void HandleKeyPressed( unsigned char keyCode );
	void HandleKeyReleased( unsigned char keyCode );
	XboxController const& GetController( int controllerID );

	static bool Event_KeyPressed( EventArgs& args );
	static bool Event_KeyReleased( EventArgs& args );
	static bool Event_CharacterInput( EventArgs& args );

	InputConfig m_config;

protected:
	KeyButtonState m_keyStates[ NUM_KEYCODES ];
	XboxController m_controllers[NUM_XBOX_CONTROLLERS] = 
	{ 
		XboxController( 0 ), 
		XboxController( 1 ), 
		XboxController( 2 ), 
		XboxController( 3 ) 
	};
};