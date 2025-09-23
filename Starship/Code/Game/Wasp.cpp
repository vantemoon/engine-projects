#include "Engine//Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Wasp.hpp"


//-----------------------------------------------------------------------------------------------
Wasp::Wasp( Game* game, Vec2 const& startingPosition )
	: Entity( game, startingPosition )
{
	Vec2 toPlayer = m_game->m_playerShip->m_position - m_position;
	m_orientationDegrees = toPlayer.GetOrientationDegrees();
	m_velocity = WASP_SPEED * toPlayer.GetNormalized();

	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	m_health = 2;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Wasp::~Wasp()
{
	delete[] m_vertexArray;
	m_vertexArray = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Wasp::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	Vec2 toPlayer = m_game->m_playerShip->m_position - m_position;
	m_orientationDegrees = toPlayer.GetOrientationDegrees();

	Accelerate( deltaSeconds );

	this->CheckCollisionWithBullets();
	this->CheckCollisionWithPlayerShip();
}


//-----------------------------------------------------------------------------------------------
void Wasp::Render() const
{
	/// Create a copy of the beetle's vertex array to transform
	Vertex tempWaspWorldVerts[NUM_WASP_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++ vertIndex )
	{
		tempWaspWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}
	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_WASP_VERTS, tempWaspWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_WASP_VERTS, tempWaspWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Wasp::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_WASP_VERTS];

	m_vertexArray[0].m_position = Vec3( -1.f, 1.2f, 0.f );
	m_vertexArray[1].m_position = Vec3( -0.5f, 0.f, 0.f );
	m_vertexArray[2].m_position = Vec3( 1.5f, 0.f, 0.f );
	m_vertexArray[0].m_color = Rgba8( 255, 255, 0 );
	m_vertexArray[1].m_color = Rgba8( 255, 255, 0 );
	m_vertexArray[2].m_color = Rgba8( 255, 255, 0 );

	m_vertexArray[3].m_position = Vec3( -1.f, -1.2f, 0.f );
	m_vertexArray[4].m_position = Vec3( -0.5f, 0.f, 0.f );
	m_vertexArray[5].m_position = Vec3( 1.5f, 0.f, 0.f );
	m_vertexArray[3].m_color = Rgba8( 255, 255, 0 );
	m_vertexArray[4].m_color = Rgba8( 255, 255, 0 );
	m_vertexArray[5].m_color = Rgba8( 255, 255, 0 );
}


//-----------------------------------------------------------------------------------------------
void Wasp::Accelerate( float deltaSeconds )
{
	Vec2 forwardNormal = this->GetForwardNormal();
	m_velocity += forwardNormal * WASP_ACCELERATION * deltaSeconds;
	float speed = m_velocity.GetLength();
	if ( speed > WASP_MAX_SPEED )
	{
		m_velocity = m_velocity.GetNormalized() * WASP_MAX_SPEED;
	}
}