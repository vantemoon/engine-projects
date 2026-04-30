#include "Game/Player.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Game* owner, ControlMode controlMode )
	: m_controlMode( controlMode )
{
	m_game = owner;

	if ( m_controlMode == ControlMode::KEYBOARD )
	{
		m_controllerIndex = -1;
	}
	else
	{
		m_controllerIndex = 0;
	}

	m_playerWorldCamera = new Camera();
	m_playerScreenCamera = new Camera();

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
	m_playerWorldCamera->SetCameraToRenderTransform( cameraToRender );
	m_playerWorldCamera->SetPerspectiveView( g_engine->m_window->m_config.m_clientAspect, 60.f, 0.1f, 100.f );
	m_playerScreenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );
}


//-----------------------------------------------------------------------------------------------
Player::~Player()
{
	delete m_playerWorldCamera;
	m_playerWorldCamera = nullptr;

	delete m_playerScreenCamera;
	m_playerScreenCamera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( m_game != nullptr && m_game->m_currentGameState != GameState::PLAYING )
	{
		return;
	}

	Actor* actor = GetActor();

	if ( actor == nullptr )
	{
		if ( m_map != nullptr && m_possessedActor.IsValid() )
		{
			m_map->SpawnPlayer( this );
		}
		return;
	}

	if ( actor->m_isDead )
	{
		UpdateCamera();

		if ( actor->m_isDestroyed )
		{
			if ( m_map != nullptr )
			{
				m_map->RespawnPlayer( this );
			}
		}

		return;
	}

	UpdateInput();
	UpdateCamera();
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateInput()
{
	float deltaSeconds = ( float ) Clock::GetSystemClock().GetDeltaSeconds();

	bool multiplayer = false;
	if ( m_game != nullptr )
	{
		multiplayer = ( ( int ) m_game->m_players.size() > 1 );
	}

	// Disable free-fly in multiplayer
	if ( !multiplayer && m_controlMode == ControlMode::KEYBOARD )
	{
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'F' ) )
		{
			if ( m_cameraMode == CameraMode::FREE_FLY )
			{
				m_cameraMode = CameraMode::FIRST_PERSON;

				Actor* actor = GetActor();
				if ( actor == nullptr || actor->m_definition == nullptr )
				{
					return;
				}

				if ( actor->m_isDead )
				{
					return;
				}

				if ( actor != nullptr )
				{
					m_orientation = actor->m_orientation;
					m_orientation.m_rollDegrees = 0.f;
				}
			}
			else
			{
				m_cameraMode = CameraMode::FREE_FLY;
			}
		}
	}

	if ( m_cameraMode == CameraMode::FREE_FLY )
	{
		UpdateFreeFlyCameraControls( deltaSeconds );
		return;
	}

	if ( m_cameraMode == CameraMode::FIRST_PERSON )
	{
		if ( m_game != nullptr && m_game->m_currentGameState == GameState::PAUSED )
		{
			return;
		}

		// Disable debug possession in multiplayer
		if ( !multiplayer && m_controlMode == ControlMode::KEYBOARD )
		{
			if ( g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) && m_map != nullptr )
			{
				m_map->DebugPossessNext();
			}
		}

		Actor* actor = GetActor();
		if ( actor == nullptr || actor->m_definition == nullptr )
		{
			return;
		}

		if ( actor->m_isDead )
		{
			return;
		}

		UpdateFirstPersonControls( actor, deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateCamera()
{
	if ( m_playerWorldCamera == nullptr ) return;

	float aspect = m_playerWorldCamera->GetViewportAspect(
		( float ) g_engine->m_window->GetClientDimensions().x,
		( float ) g_engine->m_window->GetClientDimensions().y );

	if ( m_cameraMode == CameraMode::FREE_FLY )
	{
		m_playerWorldCamera->SetPerspectiveView( aspect, 60.f, 0.1f, 100.f );
		m_playerWorldCamera->SetPositionAndOrientation( m_position, m_orientation );
	}
	else if ( m_cameraMode == CameraMode::FIRST_PERSON )
	{
		Actor* actor = GetActor();
		if ( actor == nullptr || actor->m_definition == nullptr )
		{
			return;
		}

		float eyeHeight = actor->m_definition->m_eyeHeight;

		if ( actor->m_isDead )
		{
			float corpseLifetime = ( float ) actor->m_deadTimer.m_period;
			if ( corpseLifetime > 0.f )
			{
				float elapsed = ( float ) actor->m_deadTimer.GetElapsedSeconds();
				float dropFraction = GetClamped( elapsed / ( corpseLifetime * 0.5f ), 0.f, 1.f );
				eyeHeight *= ( 1.f - dropFraction );
			}
			else
			{
				eyeHeight = 0.f;
			}
		}

		Vec3 eyePos = actor->m_position;
		eyePos.z += eyeHeight;

		m_position = eyePos;
		m_orientation.m_rollDegrees = 0.f;

		m_playerWorldCamera->SetPerspectiveView(
			aspect,
			actor->m_definition->m_cameraFOVDegrees,
			0.1f,
			100.f );

		m_playerWorldCamera->SetPositionAndOrientation( eyePos, m_orientation );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFreeFlyCameraControls( float deltaSeconds )
{
	UpdateFreeFlyCameraFromMouse();
	UpdateFreeFlyCameraFromKeyboard( deltaSeconds );
	UpdateFreeFlyCameraFromController( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFreeFlyCameraFromMouse()
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
void Player::UpdateFreeFlyCameraFromKeyboard( float deltaSeconds )
{
	float movementSpeed = 1.f;
	float rotationSpeed = 180.f;

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
void Player::UpdateFreeFlyCameraFromController( float deltaSeconds )
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );
	Vec2 rightStickPosition = controller.GetRightJoystick().GetPosition();
	Vec2 leftStickPosition = controller.GetLeftJoystick().GetPosition();
	float rightTrigger = controller.GetRightTrigger();
	float leftTrigger = controller.GetLeftTrigger();

	float movementSpeed = 1.f;
	float rotationSpeed = 180.f;

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
	g_engine->m_renderer->SetModelConstants( GetModelToWorldTransform() );

	RenderHUD();
}


//-----------------------------------------------------------------------------------------------
void Player::RenderHUD() const
{
	Actor* actor = GetActor();
	if ( actor == nullptr || actor->m_currentWeapon == nullptr )
	{
		return;
	}

	actor->m_currentWeapon->Render( actor );
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


//-----------------------------------------------------------------------------------------------
void Player::UpdateFirstPersonControls( Actor* actor, float deltaSeconds )
{
	if ( actor == nullptr || actor->m_definition == nullptr )
	{
		return;
	}

	if ( actor->m_isDead || actor->m_isDestroyed )
	{
		return;
	}

	if ( m_controlMode == ControlMode::KEYBOARD )
	{
		UpdateFirstPersonFromMouse( actor );
		UpdateFirstPersonFromKeyboard( actor, deltaSeconds );
	}
	else if ( m_controlMode == ControlMode::CONTROLLER )
	{
		UpdateFirstPersonFromController( actor, deltaSeconds );
	}

	m_orientation.m_rollDegrees = 0.f;
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFirstPersonFromMouse( Actor* actor )
{
	if ( actor == nullptr )
	{
		return;
	}

	float mouseSensitivity = 0.075f;
	IntVec2 mouseDelta = g_engine->m_inputSystem->GetCursorClientDelta();

	m_orientation.m_yawDegrees -= ( float ) mouseDelta.x * mouseSensitivity;
	m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees + ( float ) mouseDelta.y * mouseSensitivity, -85.f, 85.f );
	m_orientation.m_rollDegrees = 0.f;

	actor->m_orientation.m_yawDegrees = m_orientation.m_yawDegrees;

	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_LBUTTON ) )
	{
		actor->Attack();
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFirstPersonFromKeyboard( Actor* actor, float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( actor == nullptr || actor->m_definition == nullptr )
	{
		return;
	}

	float moveX = 0.f;
	float moveY = 0.f;

	// WASD movement
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) ) moveY += 1.f;
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) ) moveX += 1.f;
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) ) moveY -= 1.f;
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) ) moveX -= 1.f;

	bool isSprinting = g_engine->m_inputSystem->IsKeyDown( KEYCODE_SHIFT );
	float moveSpeed = actor->m_definition->m_walkSpeed;
	if ( isSprinting )
	{
		moveSpeed = actor->m_definition->m_runSpeed;
	}

	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 forwardVector = orientationMat.GetIBasis3D();
	Vec3 leftVector = orientationMat.GetJBasis3D();

	forwardVector.z = 0.f;
	leftVector.z = 0.f;
	forwardVector = forwardVector.GetNormalized();
	leftVector = leftVector.GetNormalized();

	Vec3 moveDirection = ( forwardVector * moveY ) + ( leftVector * moveX );
	if ( moveDirection.GetLengthSquared() > 0.f )
	{
		actor->MoveInDirection( moveDirection, moveSpeed );
	}

	// Left arrow
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_LEFTARROW ) )
	{
		SelectPreviousWeapon( actor );
	}

	// Right arrow
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_RIGHTARROW ) )
	{
		SelectNextWeapon( actor );
	}

	// 1
	if ( g_engine->m_inputSystem->WasKeyJustPressed( '1' ) )
	{
		SelectWeaponBySlot( actor, 0 );
	}

	// 2
	if ( g_engine->m_inputSystem->WasKeyJustPressed( '2' ) )
	{
		SelectWeaponBySlot( actor, 1 );
	}
}


///-----------------------------------------------------------------------------------------------
void Player::UpdateFirstPersonFromController( Actor* actor, float deltaSeconds )
{
	if ( actor == nullptr || actor->m_definition == nullptr )
	{
		return;
	}

	if ( m_controllerIndex < 0 )
	{
		return;
	}

	XboxController const& controller = g_engine->m_inputSystem->GetController( m_controllerIndex );

	float turnSpeed = actor->m_definition->m_turnSpeed;
	Vec2 rightStick = controller.GetRightJoystick().GetPosition();
	m_orientation.m_yawDegrees -= rightStick.x * turnSpeed * deltaSeconds;
	m_orientation.m_pitchDegrees = GetClamped( m_orientation.m_pitchDegrees - rightStick.y * turnSpeed * deltaSeconds, -85.f, 85.f );
	m_orientation.m_rollDegrees = 0.f;

	if ( rightStick.x != 0.f )
	{
		EulerAngles desiredYawOnly( m_orientation.m_yawDegrees, 0.f, 0.f );
		Mat44 orientationMat = desiredYawOnly.GetAsMatrix_IFwd_JLeft_KUp();
		Vec3 desiredForward = orientationMat.GetIBasis3D();

		float maxTurnDegrees = actor->m_definition->m_turnSpeed * deltaSeconds;
		actor->TurnInDirection( desiredForward, maxTurnDegrees );
		m_orientation.m_yawDegrees = actor->m_orientation.m_yawDegrees;
	}

	Vec2 leftStick = controller.GetLeftJoystick().GetPosition();

	bool isSprinting = controller.IsButtonDown( XBOX_BUTTON_A );
	float moveSpeed = actor->m_definition->m_walkSpeed;
	if ( isSprinting )
	{
		moveSpeed = actor->m_definition->m_runSpeed;
	}

	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 forwardVector = orientationMat.GetIBasis3D();
	Vec3 leftVector = -orientationMat.GetJBasis3D();

	forwardVector.z = 0.f;
	leftVector.z = 0.f;
	forwardVector = forwardVector.GetNormalized();
	leftVector = leftVector.GetNormalized();

	// Movement
	Vec3 moveDirection = ( forwardVector * leftStick.y ) + ( leftVector * leftStick.x );
	if ( moveDirection.GetLengthSquared() > 0.f )
	{
		actor->MoveInDirection( moveDirection, moveSpeed );
	}

	// Attack
	if ( controller.GetRightTrigger() > 0.1f )
	{
		actor->Attack();
	}

	// Weapon selection
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_DOWN ) )
	{
		SelectPreviousWeapon( actor );
	}
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_UP ) )
	{
		SelectNextWeapon( actor );
	}
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_X ) )
	{
		SelectWeaponBySlot( actor, 0 );
	}
	if ( controller.WasButtonJustPressed( XBOX_BUTTON_Y ) )
	{
		SelectWeaponBySlot( actor, 1 );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::SelectPreviousWeapon( Actor* actor )
{
	if ( actor == nullptr || actor->m_inventory.empty() )
	{
		return;
	}

	int currentIndex = 0;
	for ( int index = 0; index < ( int ) actor->m_inventory.size(); ++index )
	{
		if ( &actor->m_inventory[index] == actor->m_currentWeapon )
		{
			currentIndex = index;
			break;
		}
	}

	int previousIndex = currentIndex - 1;
	if ( previousIndex < 0 )
	{
		previousIndex = ( int ) actor->m_inventory.size() - 1;
	}

	actor->EquipWeapon( &actor->m_inventory[previousIndex] );
}


//-----------------------------------------------------------------------------------------------
void Player::SelectNextWeapon( Actor* actor )
{
	if ( actor == nullptr || actor->m_inventory.empty() )
	{
		return;
	}

	int currentIndex = 0;
	for ( int index = 0; index < ( int ) actor->m_inventory.size(); ++index )
	{
		if ( &actor->m_inventory[index] == actor->m_currentWeapon )
		{
			currentIndex = index;
			break;
		}
	}

	int nextIndex = ( currentIndex + 1 ) % ( int ) actor->m_inventory.size();
	actor->EquipWeapon( &actor->m_inventory[nextIndex] );
}


//-----------------------------------------------------------------------------------------------
void Player::SelectWeaponBySlot( Actor* actor, int slotIndex )
{
	if ( actor == nullptr )
	{
		return;
	}

	if ( slotIndex < 0 || slotIndex >= ( int ) actor->m_inventory.size() )
	{
		return;
	}

	actor->EquipWeapon( &actor->m_inventory[slotIndex] );
}


//-----------------------------------------------------------------------------------------------
ControlMode Player::GetControlMode() const
{
	return m_controlMode;
}


//-----------------------------------------------------------------------------------------------
void Player::SetPlayerIndex( int playerIndex )
{
	m_playerIndex = playerIndex;
}


//-----------------------------------------------------------------------------------------------
void Player::SetControllerIndex( int controllerIndex )
{
	m_controllerIndex = controllerIndex;
}


//-----------------------------------------------------------------------------------------------
void Player::SetViewport( AABB2 const& viewport )
{
	m_viewport = viewport;

	if ( m_playerWorldCamera != nullptr )
	{
		m_playerWorldCamera->SetViewport( viewport );
	}

	if ( m_playerScreenCamera != nullptr )
	{
		m_playerScreenCamera->SetViewport( viewport );

		float screenWidth = SCREEN_SIZE_X;
		float screenHeight = SCREEN_SIZE_Y * viewport.GetDimensions().y;

		m_playerScreenCamera->SetOrthoView(
			Vec2( 0.f, 0.f ),
			Vec2( screenWidth, screenHeight ) );
	}
}


//-----------------------------------------------------------------------------------------------
bool Player::IsDead() const
{
	Actor* actor = GetActor();
	return actor != nullptr && actor->m_isDead;
}


//-----------------------------------------------------------------------------------------------
Vec3 Player::GetForwardVector() const
{
	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	return orientationMat.GetIBasis3D();
}


//-----------------------------------------------------------------------------------------------
Vec3 Player::GetUpVector() const
{
	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	return orientationMat.GetKBasis3D();
}