#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Game* owner )
	: Entity( owner )
{
	m_playerCamera = new Camera();

	float values[16] =
	{
		 0.f, 0.f, 1.f, 0.f, // Column-major order from left to right
		-1.f, 0.f, 0.f, 0.f,
		 0.f, 1.f, 0.f, 0.f,
		 0.f, 0.f, 0.f, 1.f
	};
	Mat44 cameraToRender( values );
	m_playerCamera->SetCameraToRenderTransform( cameraToRender );
	m_playerCamera->SetPerspectiveView( g_engine->m_window->m_config.m_clientAspect, 60.f, 0.1f, 100.f );
}


//-----------------------------------------------------------------------------------------------
Player::~Player()
{
	delete m_playerCamera;
	m_playerCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	UpdateCamera();
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateCamera()
{
	m_playerCamera->SetPerspectiveView( g_engine->m_window->m_config.m_clientAspect, 60.f, 0.1f, 100.f );
	m_playerCamera->SetPositionAndOrientation( m_position, m_orientation );

	float systemDeltaSeconds = ( float ) Clock::GetSystemClock().GetDeltaSeconds();
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
	m_orientation.m_yawDegrees -= ( float ) deltaX * sensitivity;

	// Pitch (clamped)
	float pitch = GetClamped( m_orientation.m_pitchDegrees + ( float ) deltaY * sensitivity, -85.f, 85.f );
	m_orientation.m_pitchDegrees = pitch;
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( float deltaSeconds )
{
	float movementSpeed = 2.f;
	float rotationSpeed = 90.f;

	// Roll (clamped)
	if ( g_engine->m_inputSystem->IsKeyDown( 'Q' ) )
	{
		float roll = GetClamped( m_orientation.m_rollDegrees + rotationSpeed * deltaSeconds, -45.f, 45.f );
		m_orientation.m_rollDegrees = roll;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'E' ) )
	{
		float roll = GetClamped( m_orientation.m_rollDegrees - rotationSpeed * deltaSeconds, -45.f, 45.f );
		m_orientation.m_rollDegrees = roll;
	}

	float movementAmount = movementSpeed * deltaSeconds;
	
	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 forwardVector = orientationMat.GetIBasis3D();
	Vec3 leftVector = orientationMat.GetJBasis3D();
	Vec3 upVector = orientationMat.GetKBasis3D();

	// Increase movement speed by 10
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_SHIFT ) )
	{
		movementSpeed *= 10.f;
		rotationSpeed *= 10.f;
	}

	// Left and right movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		m_position += leftVector * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		m_position += leftVector * -movementAmount;
	}

	// Forward and backward movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		m_position += forwardVector * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) )
	{
		m_position += forwardVector * -movementAmount;
	}

	// Up and down movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'Z' ) )
	{
		m_position += upVector * -movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'C' ) )
	{
		m_position += upVector * movementAmount;
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
	float rightTrigger = controller.GetRightTrigger();
	float leftTrigger = controller.GetLeftTrigger();

	float movementSpeed = 2.f;
	float rotationSpeed = 90.f;

	// Yaw (not clamped)
	m_orientation.m_yawDegrees -= rightStickPosition.x * rotationSpeed * deltaSeconds;

	// Pitch (clamped)
	float pitch = GetClamped( m_orientation.m_pitchDegrees - rightStickPosition.y * rotationSpeed * deltaSeconds, -85.f, 85.f );
	m_orientation.m_pitchDegrees = pitch;

	// Roll (clamped)
	float rightTriggerRoll = rightTrigger * -rotationSpeed * deltaSeconds;
	float leftTriggerRoll = leftTrigger * rotationSpeed * deltaSeconds;
	float roll = GetClamped( m_orientation.m_rollDegrees + rightTriggerRoll + leftTriggerRoll, -45.f, 45.f );
	m_orientation.m_rollDegrees = roll;

	// Increase movement speed by 10
	if ( controller.IsButtonDown( XBOX_BUTTON_A ) )
	{
		movementSpeed *= 10.f;
		rotationSpeed *= 10.f;
	}

	Vec3 forward, left, up;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp( forward, left, up );

	// Left and right movement
	m_position += left * leftStickPosition.x * movementSpeed * deltaSeconds;

	// Forward and backward movement
	m_position += forward * leftStickPosition.y * movementSpeed * deltaSeconds;

	// Up and down movement
	if ( controller.IsButtonDown( XBOX_BUTTON_LEFT_SHOULDER ) )
	{
		m_position += up * -movementSpeed * deltaSeconds;
	}
	if ( controller.IsButtonDown( XBOX_BUTTON_RIGHT_SHOULDER ) )
	{
		m_position += up * movementSpeed * deltaSeconds;
	}

	// Reset position and orientation
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_START ) )
	{
		m_position = Vec3::ZERO;
		m_orientation = EulerAngles::ZERO;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	Entity::Render();
}