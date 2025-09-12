#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
Bullet::Bullet( Game* game, PlayerShip* shooter )
	: Entity( game, Vec2( shooter->m_position.x + 1.f, shooter->m_position.y ) )
{
	m_shooter = shooter;
	m_orientationDegrees = shooter->m_orientationDegrees;
	m_velocity = 50.f * shooter->GetForwardNormal();
	m_physicsRadius = 0.5f;
	m_cosmeticRadius = 2.0f;
	m_vertexArray = new Vertex[NUM_BULLET_VERTS];

	// Triangle A
	m_vertexArray[0] = Vertex( Vec3(   0, -0.5, 0 ), Rgba8( 255, 255, 0, 255 ), Vec2() );
	m_vertexArray[1] = Vertex( Vec3( 0.5,    0, 0 ), Rgba8( 255, 255, 0, 255 ), Vec2() );
	m_vertexArray[2] = Vertex( Vec3(   0,  0.5, 0 ), Rgba8( 255, 255, 0, 255 ), Vec2() );

	// Triangle B
	m_vertexArray[3] = Vertex( Vec3(  0, -0.5, 0 ), Rgba8( 255, 0, 0, 255 ), Vec2() );
	m_vertexArray[4] = Vertex( Vec3(  0,  0.5, 0 ), Rgba8( 255, 0, 0, 255 ), Vec2() );
	m_vertexArray[5] = Vertex( Vec3( -2,    0, 0 ), Rgba8( 255, 0, 0,   0 ), Vec2() );
}


//--------------------------------------------------------------------------------
Bullet::~Bullet() = default;


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	if ( IsOffScreen() )
	{
		Die();
	}
}


//-----------------------------------------------------------------------------------------------
void Bullet::Render() const
{
	// Create a copy of the ship's vertex array to transform
	Vertex tempShipWorldVerts[NUM_BULLET_VERTS];
	for(int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}

	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_BULLET_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_BULLET_VERTS, tempShipWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Bullet::Die()
{
	Entity::Die();
	m_isGarbage = true;
}


//-----------------------------------------------------------------------------------------------
bool Bullet::IsOffScreen() const
{
	float screenLeft = 0.f;
	float screenRight = 200.f;
	float screenBottom = 0.f;
	float screenTop = 100.f;

	if ( m_position.x + m_cosmeticRadius < screenLeft || m_position.x - m_cosmeticRadius > screenRight ||
		 m_position.y + m_cosmeticRadius < screenBottom || m_position.y - m_cosmeticRadius > screenTop )
	{
		return true;
	}
	return false;
}