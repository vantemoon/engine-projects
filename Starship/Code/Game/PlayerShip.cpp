#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
PlayerShip::PlayerShip( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity )
	: Entity( game, startingPosition )
{
	m_velocity = startingVelocity;
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	m_vertexArray = new Vertex[NUM_SHIP_VERTS];

	// Triangle A
	m_vertexArray[0] = Vertex( Vec3(  2, 1, 0 ), Rgba8( 102, 153, 204, 255), Vec2() );
	m_vertexArray[1] = Vertex( Vec3(  0, 2, 0 ), Rgba8( 102, 153, 204, 255), Vec2() );
	m_vertexArray[2] = Vertex( Vec3( -2, 1, 0 ), Rgba8( 102, 153, 204, 255), Vec2() );

	// Triangle B
	m_vertexArray[3] = Vertex( Vec3(  0,  1, 0 ), Rgba8( 102, 153, 204, 255), Vec2() );
	m_vertexArray[4] = Vertex( Vec3( -2,  1, 0 ), Rgba8( 102, 153, 204, 255), Vec2() );
	m_vertexArray[5] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255), Vec2() );

	// Triangle C
	m_vertexArray[6] = Vertex( Vec3(  0,  1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[7] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[8] = Vertex( Vec3(  0, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle D
	m_vertexArray[9] =  Vertex( Vec3( 1,  0, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[10] = Vertex( Vec3( 0,  1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[11] = Vertex( Vec3( 0, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );

	// Triangle E
	m_vertexArray[12] = Vertex( Vec3(  2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[13] = Vertex( Vec3( -2, -1, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
	m_vertexArray[14] = Vertex( Vec3(  0, -2, 0 ), Rgba8( 102, 153, 204, 255 ), Vec2() );
}

//--------------------------------------------------------------------------------
PlayerShip::~PlayerShip() = default;


//-----------------------------------------------------------------------------------------------
void PlayerShip::Update( float deltaSeconds )
{
	if( m_isDead )
		return;

	Entity::Update( deltaSeconds );

	if( m_isAccelerating )
	{
		Accelerate( deltaSeconds );
	}

	BounceOffWorldEdges();

	if( m_isTurningLeft )
	{
		TurnLeft();
	}
	else if( m_isTurningRight )
	{
		TurnRight();
	}
	else
	{
		m_angularVelocityDegreesPerSecond = 0.f;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Render() const
{
	if ( m_isDead )
		return;

	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++ vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}

	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_SHIP_VERTS, tempShipWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Die()
{
	Entity::Die(); // m_isDead = true;
	m_health = 0;
	m_velocity = Vec2( 0.f, 0.f );
	m_angularVelocityDegreesPerSecond = 0.f;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::BounceOffWorldEdges()
{
	float screenLeft = 0.f + m_physicsRadius;
	float screenRight = WORLD_SIZE_X - m_physicsRadius;
	float screenBottom = 0.f + m_physicsRadius;
	float screenTop = WORLD_SIZE_Y - m_physicsRadius;

	if ( m_position.x < screenLeft )
	{
		m_position.x = screenLeft;
		m_velocity.x = -m_velocity.x;
	}
	else if ( m_position.x > screenRight )
	{
		m_position.x = screenRight;
		m_velocity.x = -m_velocity.x;
	}
	if ( m_position.y < screenBottom )
	{
		m_position.y = screenBottom;
		m_velocity.y = -m_velocity.y;
	}
	else if ( m_position.y > screenTop )
	{
		m_position.y = screenTop;
		m_velocity.y = -m_velocity.y;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Accelerate( float deltaSeconds )
{
	Vec2 forwardNormal = GetForwardNormal();
	float accelerationRate = 30.f;
	m_velocity += forwardNormal * accelerationRate * deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::TurnLeft()
{
	m_angularVelocityDegreesPerSecond = PLAYER_SHIP_TURN_SPEED;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::TurnRight()
{
	m_angularVelocityDegreesPerSecond = -PLAYER_SHIP_TURN_SPEED;
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Respawn()
{
	m_isDead = false;
	m_health = 1;
	m_position = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );
	m_velocity = Vec2( 0.f, 0.f );
	m_orientationDegrees = 0.f;
}