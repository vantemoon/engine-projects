#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Game* owner )
	: Entity( owner )
{
}


//-----------------------------------------------------------------------------------------------
Player::~Player()
{
}


//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	float systemDeltaSeconds = ( float ) m_game->m_gameClock->GetDeltaSeconds();
	UpdateFromMouse();
	UpdateFromKeyboard( systemDeltaSeconds );
	UpdateFromController( systemDeltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromMouse()
{
	float sensitivity = 0.125f;
	IntVec2 mouseDelta = g_engine->m_inputSystem->GetCursorClientDelta();
	int deltaX = mouseDelta.x;
	int deltaY = mouseDelta.y;

	// Yaw (not clamped)
	m_orientation.m_yawDegrees += ( float ) deltaX * sensitivity;

	// Pitch (clamped)
	float pitch = GetClamped( m_orientation.m_pitchDegrees + ( float ) deltaY * sensitivity, -85.f, 85.f );
	m_orientation.m_pitchDegrees = pitch;
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( float deltaSeconds )
{
	// Roll (clamped)
	if ( g_engine->m_inputSystem->IsKeyDown( 'Q' ) )
	{
		float roll = GetClamped( m_orientation.m_rollDegrees - 90.f * deltaSeconds, -45.f, 45.f );
		m_orientation.m_rollDegrees = roll;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'E' ) )
	{
		float roll = GetClamped( m_orientation.m_rollDegrees + 90.f * deltaSeconds, -45.f, 45.f );
		m_orientation.m_rollDegrees = roll;
	}

	float movementSpeed = 2.f;

	// Increase movement speed by 10
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SHIFT ) )
	{
		movementSpeed *= 10.f;
	}

	float movementAmount = movementSpeed * deltaSeconds;
	Vec3 forward, left, up;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp( forward, left, up );

	// Left and right movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		m_position += left * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		m_position += left * -movementAmount;
	}

	// Forward and backward movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		m_position += forward * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) )
	{
		m_position += forward * -movementAmount;
	}

	// Up and down movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'Z' ) )
	{
		m_position += up * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'C' ) )
	{
		m_position += up * -movementAmount;
	}

	// Reset position and orientation
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'H' ) )
	{
		m_position = Vec3::ZERO;
		m_orientation = EulerAngles::ZERO;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromController( float deltaSeconds )
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );
	Vec2 rightStickPosition = controller.GetRightJoystick().GetPosition();
	Vec2 leftStickPosition = controller.GetLeftJoystick().GetPosition();

	// Yaw (not clamped)
	m_orientation.m_yawDegrees += rightStickPosition.x * 90.f * deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
}