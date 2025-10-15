#include "Game/Bullet.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Game* game, PlayerShip* shooter )
	: Entity( game, Vec2( shooter->m_position.x + 1.f, shooter->m_position.y ) )
{
	m_shooter = shooter;
	m_orientationDegrees = shooter->m_orientationDegrees;
	m_velocity = BULLET_SPEED * shooter->GetForwardNormal();
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_ageSeconds = 0.0f;
	
	InitializeVertexArray();
}


//--------------------------------------------------------------------------------
void Bullet::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_BULLET_VERTS];

	// Triangle A (front, yellow)
	m_vertexArray[0] = Vertex( Vec3( 0.0f, -0.75f, 0.0f ), Rgba8( 255, 255, 0, 255 ), Vec2() );
	m_vertexArray[1] = Vertex( Vec3( 0.75f, 0.0f, 0.0f ), Rgba8( 255, 255, 0, 255 ), Vec2() );
	m_vertexArray[2] = Vertex( Vec3( 0.0f, 0.75f, 0.0f ), Rgba8( 255, 255, 0, 255 ), Vec2() );

	// Triangle B (rear, red)
	m_vertexArray[3] = Vertex( Vec3( 0.0f, -0.75f, 0.0f ), Rgba8( 255, 0, 0, 255 ), Vec2() );
	m_vertexArray[4] = Vertex( Vec3( 0.0f, 0.75f, 0.0f ), Rgba8( 255, 0, 0, 255 ), Vec2() );
	m_vertexArray[5] = Vertex( Vec3( -3.0f, 0.0f, 0.0f ), Rgba8( 255, 0, 0, 0 ), Vec2() );
}


//--------------------------------------------------------------------------------
Bullet::~Bullet() = default;


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	m_ageSeconds += deltaSeconds;
	if ( m_ageSeconds > BULLET_LIFETIME_SECONDS )
	{
		Die( false );
	}

	if ( IsOffScreen() )
	{
		Die( true );
	}
}


//-----------------------------------------------------------------------------------------------
void Bullet::Render() const
{
	// Create a copy of the ship's vertex array to transform
	Vertex tempShipWorldVerts[NUM_BULLET_VERTS];
	for( int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; ++ vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}

	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_BULLET_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_BULLET_VERTS, tempShipWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Bullet::Die( bool shouldSpawnDebris )
{
	Entity::Die();

	if ( shouldSpawnDebris )
	{
		m_game->SpawnDebrisCluster( 3, m_position, m_velocity * -0.2f, Rgba8( 255, 128, 0 ), m_cosmeticRadius * 0.05f, m_cosmeticRadius * 0.1f );

	}
}