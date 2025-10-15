#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"


//-----------------------------------------------------------------------------------------------
enum XboxButtonID
{
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,
	XBOX_BUTTON_UP,
	XBOX_BUTTON_DOWN,
	XBOX_BUTTON_LEFT,
	XBOX_BUTTON_RIGHT,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
	XBOX_BUTTON_LEFT_THUMB,
	XBOX_BUTTON_RIGHT_THUMB,
	XBOX_BUTTON_LEFT_SHOULDER,
	XBOX_BUTTON_RIGHT_SHOULDER,
	NUM_XBOX_BUTTONS
};


//-----------------------------------------------------------------------------------------------
class XboxController
{
	friend class InputSystem;

public:
	XboxController( int controllerID );
	~XboxController();
	bool                  IsConnected()										const;
	int                   GetControllerID()									const;
	AnalogJoystick const& GetLeftJoystick()									const;
	AnalogJoystick const& GetRightJoystick()								const;
	float                 GetLeftTrigger()									const;
	float                 GetRightTrigger()                                 const;
	KeyButtonState const& GetButton( XboxButtonID buttonID )                const;
	bool                  IsButtonDown( XboxButtonID buttonID )             const;
	bool                  WasButtonJustPressed( XboxButtonID buttonID )     const;
	bool                  WasButtonJustReleased( XboxButtonID buttonID )    const;

private:
	void				  Update();
	void				  Reset();
	void				  UpdateJoystick( AnalogJoystick& out_joystick, short rawX, short rawY );
	void				  UpdateTrigger( float& out_triggerValue, unsigned char rawValue );
	void			      UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag );

private:
	int					  m_controllerID = -1;
	bool				  m_isConnected = false;
	float				  m_leftTrigger = 0.f;
	float				  m_rightTrigger = 0.f;
	KeyButtonState		  m_buttons[NUM_XBOX_BUTTONS];
	AnalogJoystick		  m_leftJoystick;
	AnalogJoystick		  m_rightJoystick;
};