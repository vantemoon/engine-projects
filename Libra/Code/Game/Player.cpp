#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Vec2 startingPosition )
	: Entity( startingPosition )
{
	m_physicsRadius = PLAYER_TANK_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_TANK_COSMETIC_RADIUS;
	m_orientationDegrees = 45.f;
	m_turretOrientationDegrees = m_orientationDegrees;
	m_turretRelativeDegrees = 0.f;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Player::~Player() = default;


//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	UpdateFromKeyboard( deltaSeconds );
	UpdateFromController( deltaSeconds );
	TurnTowardMovementDirection( deltaSeconds );
	TurnTurretTowardAimDirection( deltaSeconds );

	m_velocity = m_thrustFraction * PLAYER_TANK_MAX_SPEED_TILES_PER_SECOND * GetForwardNormal();
	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	// Movement
	bool isWKeyDown = g_engine->m_inputSystem->IsKeyDown( 'W' );
	bool isAKeyDown = g_engine->m_inputSystem->IsKeyDown( 'A' );
	bool isSKeyDown = g_engine->m_inputSystem->IsKeyDown( 'S' );
	bool isDKeyDown = g_engine->m_inputSystem->IsKeyDown( 'D' );

	bool isAnyMovementKeyDown = isWKeyDown || isAKeyDown || isSKeyDown || isDKeyDown;

	if ( isAnyMovementKeyDown )
	{
		m_isMovingForward = true;
		m_thrustFraction = 1.f;

		float totalDeltaOrientation = 0.f;
		int numKeysPressed = 0;
		if ( isWKeyDown ) {
			totalDeltaOrientation += 90.f;   
			numKeysPressed++;
		}
		if ( isAKeyDown ) {
			totalDeltaOrientation += 180.f;  
			numKeysPressed++;
		}
		if ( isSKeyDown ) {
			totalDeltaOrientation += 270.f;  
			numKeysPressed++;
		}
		if ( isDKeyDown )
		{
			if ( isWKeyDown )
			{
				totalDeltaOrientation += 0.f;
			}
			else if ( isSKeyDown )
			{
				totalDeltaOrientation += 360.f;
			}
			else
			{
				totalDeltaOrientation += 0.f;
			}
			numKeysPressed++;
		}
		float deltaOrientation = totalDeltaOrientation / ( float ) numKeysPressed;
		m_targetMovementDirection = deltaOrientation;
	}
	else
	{
		m_isMovingForward = false;
		m_thrustFraction = 0.f;
	}

	// Turret Aiming
	bool isIKeyDown = g_engine->m_inputSystem->IsKeyDown( 'I' );
	bool isJKeyDown = g_engine->m_inputSystem->IsKeyDown( 'J' );
	bool isKKeyDown = g_engine->m_inputSystem->IsKeyDown( 'K' );
	bool isLKeyDown = g_engine->m_inputSystem->IsKeyDown( 'L' );

	bool isAiming = isIKeyDown || isJKeyDown || isKKeyDown || isLKeyDown;

	if ( isAiming ) {
		float totalDeltaOrientation = 0.f;
		int numKeysPressed = 0;

		if ( isIKeyDown ) {
			totalDeltaOrientation += 90.f;   
			numKeysPressed++;
		}
		if ( isJKeyDown ) {
			totalDeltaOrientation += 180.f;  
			numKeysPressed++;
		}
		if ( isKKeyDown ) {
			totalDeltaOrientation += 270.f;  
			numKeysPressed++;
		}
		if ( isLKeyDown )
		{
			if ( isIKeyDown )
			{
				totalDeltaOrientation += 0.f;
			}
			else if ( isKKeyDown )
			{
				totalDeltaOrientation += 360.f;
			}
			else
			{
				totalDeltaOrientation += 0.f;
			}
			numKeysPressed++;
		}
		float deltaOrientation = totalDeltaOrientation / ( float ) numKeysPressed;
		m_turretTargetDegrees = deltaOrientation;

		m_isTurretAiming = true;
	} 
	else 
	{
		m_turretTargetDegrees = m_orientationDegrees + m_turretRelativeDegrees;
		m_isTurretAiming = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateFromController( [[maybe_unused]] float deltaSeconds )
{
	XboxController const& controller = g_engine->m_inputSystem->GetController( 0 );

	// Movement
	float leftJoystickMagnitude = controller.GetLeftJoystick().GetMagnitude();
	float leftJoystickOrientationDegrees = controller.GetLeftJoystick().GetOrientationDegrees();
	if ( leftJoystickMagnitude > 0.f )
	{
		m_isMovingForward = true;
		m_thrustFraction = leftJoystickMagnitude;
		m_targetMovementDirection = leftJoystickOrientationDegrees;
	}

	// Turret Aiming
	float rightJoystickMagnitude = controller.GetRightJoystick().GetMagnitude();
	float rightJoystickOrientationDegrees = controller.GetRightJoystick().GetOrientationDegrees();
	if ( rightJoystickMagnitude > 0.f )
	{
		m_turretTargetDegrees = rightJoystickOrientationDegrees;
		m_isTurretAiming = true;
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	if ( m_isDead )
	{
		return;
	}

    // Body
    Vertex tempBodyVerts[NUM_PLAYER_TANK_BODY_VERTS];
    for (int i = 0; i < NUM_PLAYER_TANK_BODY_VERTS; ++i) {
        tempBodyVerts[i] = m_vertexArray[i];
    }
    TransformVertexArrayXY3D(NUM_PLAYER_TANK_BODY_VERTS, tempBodyVerts, 1.f, m_orientationDegrees, m_position);
    g_engine->m_renderer->DrawVertexArray(NUM_PLAYER_TANK_BODY_VERTS, tempBodyVerts);

    // Turret
    Vertex tempTurretVerts[NUM_PLAYER_TANK_TURRET_VERTS];
    for (int i = 0; i < NUM_PLAYER_TANK_TURRET_VERTS; ++i) {
        tempTurretVerts[i] = m_vertexArray[NUM_PLAYER_TANK_BODY_VERTS + i];
    }

    TransformVertexArrayXY3D(NUM_PLAYER_TANK_TURRET_VERTS, tempTurretVerts, 1.f, m_turretOrientationDegrees - m_orientationDegrees, Vec2(0.f, 0.f));
    TransformVertexArrayXY3D(NUM_PLAYER_TANK_TURRET_VERTS, tempTurretVerts, 1.f, m_orientationDegrees, m_position);
    g_engine->m_renderer->DrawVertexArray(NUM_PLAYER_TANK_TURRET_VERTS, tempTurretVerts);
}


//-----------------------------------------------------------------------------------------------
void Player::TakeDamage( int damage )
{
	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Player::Die()
{
	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Player::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_PLAYER_TANK_BODY_VERTS + NUM_PLAYER_TANK_TURRET_VERTS];

	// Body
	Vec3 bottomLeft  = Vec3( -m_cosmeticRadius, -m_cosmeticRadius, 0.f );
	Vec3 bottomRight = Vec3(  m_cosmeticRadius, -m_cosmeticRadius, 0.f );
	Vec3 topRight    = Vec3(  m_cosmeticRadius,  m_cosmeticRadius, 0.f );
	Vec3 topLeft     = Vec3( -m_cosmeticRadius,  m_cosmeticRadius, 0.f );

	m_vertexArray[0] = Vertex( bottomLeft,  Rgba8( 0, 255, 0 ), Vec2() );
	m_vertexArray[1] = Vertex( bottomRight, Rgba8( 0, 255, 0 ), Vec2() );
	m_vertexArray[2] = Vertex( topRight,    Rgba8( 0, 255, 0 ), Vec2() );
	m_vertexArray[3] = Vertex( topRight,    Rgba8( 0, 255, 0 ), Vec2() );
	m_vertexArray[4] = Vertex( topLeft,     Rgba8( 0, 255, 0 ), Vec2() );
	m_vertexArray[5] = Vertex( bottomLeft,  Rgba8( 0, 255, 0 ), Vec2() );

	// Turret
	float turretWidth = m_cosmeticRadius * 0.5f;
	float turretLength = m_cosmeticRadius * 0.9f;
	float halfWidth = turretWidth * 0.5f;

	Vec3 turretBottomLeft = Vec3(0.f, -halfWidth, 0.f);
	Vec3 turretBottomRight = Vec3(turretLength, -halfWidth, 0.f);
	Vec3 turretTopRight = Vec3(turretLength, halfWidth, 0.f);
	Vec3 turretTopLeft = Vec3(0.f, halfWidth, 0.f);

	m_vertexArray[6]  = Vertex( turretBottomLeft,  Rgba8( 0, 200, 0 ), Vec2() );
	m_vertexArray[7]  = Vertex( turretBottomRight,  Rgba8( 0, 200, 0 ), Vec2() );
	m_vertexArray[8]  = Vertex( turretTopRight,  Rgba8( 0, 200, 0 ), Vec2() );
	m_vertexArray[9]  = Vertex( turretTopRight,  Rgba8( 0, 200, 0 ), Vec2() );
	m_vertexArray[10] = Vertex( turretTopLeft,  Rgba8( 0, 200, 0 ), Vec2() );
	m_vertexArray[11] = Vertex( turretBottomLeft,  Rgba8( 0, 200, 0 ), Vec2() );
}


//-----------------------------------------------------------------------------------------------
void Player::TurnTowardMovementDirection( float deltaSeconds )
{
	if ( !m_isMovingForward )
		return;

	if ( m_targetMovementDirection == m_orientationDegrees )
		return;
	
	float currentOrientationDegrees = m_orientationDegrees;
	float deltaOrientationDegrees = m_targetMovementDirection - currentOrientationDegrees;
	deltaOrientationDegrees = GetShortestAngularDispDegrees( currentOrientationDegrees, m_targetMovementDirection );
	
	float maxDeltaThisFrame = PLAYER_TANK_TURN_SPEED_DEGREES_PER_SECOND * deltaSeconds;
	if ( fabsf( deltaOrientationDegrees ) <= maxDeltaThisFrame )
	{
		m_orientationDegrees = m_targetMovementDirection;
	}
	else
	{
		float deltaToApply;
		if ( deltaOrientationDegrees > 0.f )
		{
			deltaToApply = maxDeltaThisFrame;
		}
		else
		{
			deltaToApply = -maxDeltaThisFrame;
		}
		m_orientationDegrees += deltaToApply;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::TurnTurretTowardAimDirection( float deltaSeconds )
{
    if ( m_turretOrientationDegrees == m_turretTargetDegrees )
        return;

    float currentTurretDegrees = m_turretOrientationDegrees;
    float deltaTurretDegrees = GetShortestAngularDispDegrees( currentTurretDegrees, m_turretTargetDegrees );

    float maxDeltaThisFrame = PLAYER_TANK_TURRET_TURN_SPEED_DEGREES_PER_SECOND * deltaSeconds;
    if ( fabsf( deltaTurretDegrees ) <= maxDeltaThisFrame )
    {
        m_turretOrientationDegrees = m_turretTargetDegrees;
    }
    else
    {
        m_turretOrientationDegrees += (deltaTurretDegrees > 0.f ? maxDeltaThisFrame : -maxDeltaThisFrame);
    }

    if ( m_isTurretAiming ) {
        m_turretRelativeDegrees = m_turretOrientationDegrees - m_orientationDegrees;
    }
}
