#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Game* owner )
{
	m_game = owner;
	m_playerCamera = new Camera();

	m_position = Vec3( 3.f, 3.f, 10.f );
	m_orientation = EulerAngles( 45.f, 30.f, 0.f );

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

	float deltaSeconds = ( float ) Clock::GetSystemClock().GetDeltaSeconds();
	UpdateFromMouse();
	UpdateFromKeyboard( deltaSeconds );
	UpdateFromController( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromMouse()
{
	float sensitivity = 0.075f;

	IntVec2 mouseDelta = g_engine->m_inputSystem->GetCursorClientDelta();
	int deltaX = mouseDelta.x;
	int deltaY = mouseDelta.y;

	// Yaw
	m_orientation.m_yawDegrees -= ( float ) deltaX * sensitivity;

	// Pitch (clamped)
	m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees + ( float ) deltaY * sensitivity, -85.f, 85.f );
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( float deltaSeconds )
{
	float movementSpeed = 1.f;
	float rotationSpeed = 90.f;

	// Sprint
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_SHIFT ) )
	{
		movementSpeed = 15.f;
	}

	// Roll
	if ( g_engine->m_inputSystem->IsKeyDown( 'Q' ) )
	{
		m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees + rotationSpeed * deltaSeconds, -45.f, 45.f );
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'E' ) )
	{
		m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees - rotationSpeed * deltaSeconds, -45.f, 45.f );
	}

	float movementAmount = movementSpeed * deltaSeconds;

	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 forwardVector = orientationMat.GetIBasis3D();
	Vec3 leftVector = orientationMat.GetJBasis3D();
	Vec3 upVector = orientationMat.GetKBasis3D();

	// Left/right
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		m_position += leftVector * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		m_position += leftVector * -movementAmount;
	}

	// Forward/back
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		m_position += forwardVector * movementAmount;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) )
	{
		m_position += forwardVector * -movementAmount;
	}

	// Down/up
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

	float movementSpeed = 1.f;
	float rotationSpeed = 90.f;

	// Sprint
	if ( controller.IsButtonDown( XBOX_BUTTON_A ) )
	{
		movementSpeed = 15.f;
	}

	// Yaw
	m_orientation.m_yawDegrees -= rightStickPosition.x * rotationSpeed * deltaSeconds;

	// Pitch (clamped)
	m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees - rightStickPosition.y * rotationSpeed * deltaSeconds, -85.f, 85.f );

	// Roll
	float rightTriggerRoll = rightTrigger * -rotationSpeed * deltaSeconds;
	float leftTriggerRoll = leftTrigger * rotationSpeed * deltaSeconds;
	m_orientation.m_rollDegrees = GetClamped( m_orientation.m_rollDegrees + rightTriggerRoll + leftTriggerRoll, -45.f, 45.f );

	Vec3 forward, left, up;
	m_orientation.GetAsVectors_IFwd_JLeft_KUp( forward, left, up );

	// Left/right
	m_position += left * leftStickPosition.x * movementSpeed * deltaSeconds;

	// Forward/back
	m_position += forward * leftStickPosition.y * movementSpeed * deltaSeconds;

	// Down/up
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
	g_engine->m_renderer->SetModelConstants( GetModelToWorldTransform(), m_color );
}


//-----------------------------------------------------------------------------------------------
Mat44 Player::GetModelToWorldTransform() const
{
	Mat44 modelToWorld;
	Mat44 translation = Mat44::MakeTranslation3D( m_position );
	Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	modelToWorld.Append( translation );
	modelToWorld.Append( rotation );
	return modelToWorld;
}