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

	if ( m_isMovingForward )
	{
		m_thrustFraction = 1.f;
	}
	else
	{
		m_thrustFraction = 0.f;
	}
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

	if ( ( isWKeyDown || isAKeyDown || isSKeyDown || isDKeyDown ) && 
		!( isWKeyDown && isSKeyDown ) && !( isAKeyDown && isDKeyDown ) )
	{
		m_isMovingForward = true;

		if ( isDKeyDown && !isWKeyDown && !isSKeyDown )
		{
			m_targetMovementDirection = 0.f;
		}
		else if ( isWKeyDown && !isAKeyDown && !isDKeyDown )
		{
			m_targetMovementDirection = 90.f;
		}
		else if ( isAKeyDown && !isWKeyDown && !isSKeyDown )
		{
			m_targetMovementDirection = 180.f;
		}
		else if ( isSKeyDown && !isAKeyDown && !isDKeyDown )
		{
			m_targetMovementDirection = 270.f;
		}
		else if ( isWKeyDown && isDKeyDown )
		{
			m_targetMovementDirection = 45.f;
		}
		else if ( isWKeyDown && isAKeyDown )
		{
			m_targetMovementDirection = 135.f;
		}
		else if ( isSKeyDown && isAKeyDown )
		{
			m_targetMovementDirection = 225.f;
		}
		else if ( isSKeyDown && isDKeyDown )
		{
			m_targetMovementDirection = 315.f;
		}
	}
	else
	{
		m_isMovingForward = false;
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
		m_targetMovementDirection = leftJoystickOrientationDegrees;
	}
}

//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	if ( m_isDead )
	{
		return;
	}

	Vertex tempShipWorldVerts[NUM_PLAYER_TANK_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_PLAYER_TANK_VERTS; ++ vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_PLAYER_TANK_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_PLAYER_TANK_VERTS, tempShipWorldVerts );
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
	m_vertexArray = new Vertex[NUM_PLAYER_TANK_VERTS];

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