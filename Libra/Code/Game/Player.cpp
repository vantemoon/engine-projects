#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( Game* game, Vec2 startingPosition )
	: Entity( game, startingPosition )
{
	m_physicsRadius = PLAYER_TANK_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_TANK_COSMETIC_RADIUS;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Player::~Player() = default;


//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
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