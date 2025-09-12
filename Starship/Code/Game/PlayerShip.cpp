#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/App.hpp"
#include "Game/PlayerShip.hpp"


//-----------------------------------------------------------------------------------------------
PlayerShip::PlayerShip( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity )
	: Entity( game, startingPosition )
{
	m_velocity = startingVelocity;
	m_physicsRadius = 1.75f;
	m_cosmeticRadius = 2.25f;
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
	Entity::Update( deltaSeconds );
	// Add PlayerShip-specific update logic here
	// Check if the player ship has gone off the screen
	if ( IsOffScreen() )
	{
		// TODO: clamp to screen bounds
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Render() const
{
	// Create a copy of the ship's vertex array to transform
	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex )
	{
		tempShipWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}

	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_SHIP_VERTS, tempShipWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void PlayerShip::Die()
{
	Entity::Die();
	// Add PlayerShip-specific death logic here
}


//-----------------------------------------------------------------------------------------------
bool PlayerShip::IsOffScreen() const
{
	//TODO: Implement this function
	return false;
};