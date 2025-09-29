#include <Windows.h>
#include <Xinput.h>
#pragma comment( lib, "xinput" ) 

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
XboxController::XboxController()
	: m_id( -1 )
	, m_isConnected( false )
	, m_leftTrigger( 0.f )
	, m_rightTrigger( 0.f )
{
	// Initialize button states
	for ( int buttonIndex = 0; buttonIndex < NUM_XBOX_BUTTONS; ++ buttonIndex )
	{
		m_buttons[buttonIndex] = KeyButtonState();
	}

	// Set joystick dead zones
	m_leftJoystick.SetDeadZoneThresholds( 0.15f, 0.95f );
	m_rightJoystick.SetDeadZoneThresholds( 0.15f, 0.95f );
}


//-----------------------------------------------------------------------------------------------
XboxController::~XboxController() = default;


//-----------------------------------------------------------------------------------------------
bool XboxController::IsConnected() const
{
	return m_isConnected;
}


//-----------------------------------------------------------------------------------------------
int XboxController::GetControllerID() const
{
	return m_id;
}


//-----------------------------------------------------------------------------------------------
AnalogJoystick const& XboxController::GetLeftJoystick() const
{
	return m_leftJoystick;
}


//-----------------------------------------------------------------------------------------------
AnalogJoystick const& XboxController::GetRightJoystick() const
{
	return m_rightJoystick;
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetLeftTrigger() const
{
	return m_leftTrigger;
}


//-----------------------------------------------------------------------------------------------
float XboxController::GetRightTrigger() const
{
	return m_rightTrigger;
}


//-----------------------------------------------------------------------------------------------
KeyButtonState const& XboxController::GetButton( XboxButtonID buttonID ) const
{
	if ( buttonID < 0 || buttonID >= NUM_XBOX_BUTTONS )
	{
		ERROR_AND_DIE( "XboxController::GetButton() - buttonID is out of range!" );
	}
	return m_buttons[buttonID];
}


//-----------------------------------------------------------------------------------------------
bool XboxController::IsButtonDown( XboxButtonID buttonID ) const
{
	if ( buttonID < 0 || buttonID >= NUM_XBOX_BUTTONS )
	{
		ERROR_AND_DIE( "XboxController::IsButtonDown() - buttonID is out of range!" );
	}
	return m_buttons[buttonID].m_isPressed;
}


//-----------------------------------------------------------------------------------------------
bool XboxController::WasButtonJustPressed( XboxButtonID buttonID ) const
{
	if ( buttonID < 0 || buttonID >= NUM_XBOX_BUTTONS )
	{
		ERROR_AND_DIE( "XboxController::WasButtonJustPressed() - buttonID is out of range!" );
	}
	return ( m_buttons[buttonID].m_isPressed && !m_buttons[buttonID].m_wasPressedLastFrame );
}


//-----------------------------------------------------------------------------------------------
bool XboxController::WasButtonJustReleased( XboxButtonID buttonID ) const
{
	if ( buttonID < 0 || buttonID >= NUM_XBOX_BUTTONS )
	{
		ERROR_AND_DIE( "XboxController::WasButtonJustReleased() - buttonID is out of range!" );
	}
	return ( !m_buttons[buttonID].m_isPressed && m_buttons[buttonID].m_wasPressedLastFrame );
}


//-----------------------------------------------------------------------------------------------
void XboxController::Update()
{
	XINPUT_STATE xboxControllerState = {}; // Clear (zero-out) the controller state structure
	DWORD result = XInputGetState( m_id, &xboxControllerState ); // Get fresh state info
	if ( result == ERROR_SUCCESS ) // Result if the controller is connected (error code is SUCCESS)
	{
		// Update buttons
		unsigned short buttonFlags = xboxControllerState.Gamepad.wButtons;
		UpdateButton( XBOX_BUTTON_A, buttonFlags, XINPUT_GAMEPAD_A );
		UpdateButton( XBOX_BUTTON_B, buttonFlags, XINPUT_GAMEPAD_B );
		UpdateButton( XBOX_BUTTON_X, buttonFlags, XINPUT_GAMEPAD_X );
		UpdateButton( XBOX_BUTTON_Y, buttonFlags, XINPUT_GAMEPAD_Y );
		UpdateButton( XBOX_BUTTON_UP, buttonFlags, XINPUT_GAMEPAD_DPAD_UP );
		UpdateButton( XBOX_BUTTON_DOWN, buttonFlags, XINPUT_GAMEPAD_DPAD_DOWN );
		UpdateButton( XBOX_BUTTON_LEFT, buttonFlags, XINPUT_GAMEPAD_DPAD_LEFT );
		UpdateButton( XBOX_BUTTON_RIGHT, buttonFlags, XINPUT_GAMEPAD_DPAD_RIGHT );
		UpdateButton( XBOX_BUTTON_START, buttonFlags, XINPUT_GAMEPAD_START );
		UpdateButton( XBOX_BUTTON_BACK, buttonFlags, XINPUT_GAMEPAD_BACK );
		UpdateButton( XBOX_BUTTON_LEFT_THUMB, buttonFlags, XINPUT_GAMEPAD_LEFT_THUMB );
		UpdateButton( XBOX_BUTTON_RIGHT_THUMB, buttonFlags, XINPUT_GAMEPAD_RIGHT_THUMB );
		UpdateButton( XBOX_BUTTON_LEFT_SHOULDER, buttonFlags, XINPUT_GAMEPAD_LEFT_SHOULDER );
		UpdateButton( XBOX_BUTTON_RIGHT_SHOULDER, buttonFlags, XINPUT_GAMEPAD_RIGHT_SHOULDER );

		// Update joysticks
		UpdateJoystick( m_leftJoystick, xboxControllerState.Gamepad.sThumbLX, xboxControllerState.Gamepad.sThumbLY );
		UpdateJoystick( m_rightJoystick, xboxControllerState.Gamepad.sThumbRX, xboxControllerState.Gamepad.sThumbRY );

		// Update triggers
		UpdateTrigger( m_leftTrigger, xboxControllerState.Gamepad.bLeftTrigger );
		UpdateTrigger( m_rightTrigger, xboxControllerState.Gamepad.bRightTrigger );
	}
	else if ( result == ERROR_DEVICE_NOT_CONNECTED ) // Result if this controller ID# is disconnected
	{
		Reset();
		return;
	}
	else // Some other unknown error code...?
	{
		ERROR_AND_DIE( "XboxController::Update() - XInputGetState() returned an unknown error code!" );
	}
}


//-----------------------------------------------------------------------------------------------
void XboxController::Reset()
{
	m_isConnected = false;
	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;
	for ( int buttonIndex = 0; buttonIndex < NUM_XBOX_BUTTONS; ++ buttonIndex )
	{
		m_buttons[buttonIndex] = KeyButtonState();
	}
	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
}


//-----------------------------------------------------------------------------------------------
void XboxController::UpdateJoystick( AnalogJoystick& out_joystick, short rawX, short rawY )
{
	float normalizedX = RangeMapClamped( static_cast< float >( rawX ), -32768.f, 32767.f, -1.f, 1.f );
	float normalizedY = RangeMapClamped( static_cast< float >( rawY ), -32768.f, 32767.f, -1.f, 1.f );
	out_joystick.SetPosition( normalizedX, normalizedY );
}


//-----------------------------------------------------------------------------------------------
void XboxController::UpdateTrigger( float& out_triggerValue, unsigned char rawValue )
{
	out_triggerValue = RangeMapClamped( static_cast< float >( rawValue ), 0.f, 255.f, 0.f, 1.f );
}


//-----------------------------------------------------------------------------------------------
void XboxController::UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag )
{
	if ( buttonID < 0 || buttonID >= NUM_XBOX_BUTTONS )
	{
		ERROR_AND_DIE( "XboxController::UpdateButton() - buttonID is out of range!" );
	}
	// Update "was pressed last frame" state
	m_buttons[buttonID].m_wasPressedLastFrame = m_buttons[buttonID].m_isPressed;
	
	// Update "is pressed" state
	if ( ( buttonFlags & buttonFlag ) == buttonFlag )
	{
		m_buttons[buttonID].m_isPressed = true;
	}
	else
	{
		m_buttons[buttonID].m_isPressed = false;
	}
}