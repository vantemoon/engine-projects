#include "Engine//Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Beetle.hpp"


//-----------------------------------------------------------------------------------------------
Beetle::Beetle( Game* game, Vec2 const& startingPosition, float orientationDegrees, Vec2 const& startingVelocity, float startingAngularVelocity )
	: Entity( game, startingPosition )
{
	m_orientationDegrees = orientationDegrees;
	m_velocity = startingVelocity;
	m_angularVelocityDegreesPerSecond = startingAngularVelocity;
	m_physicsRadius = 2.f;
	m_cosmeticRadius = 1.75f;
	m_health = 2;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Beetle::~Beetle()
{
	delete[] m_vertexArray;
	m_vertexArray = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Beetle::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );
	// WrapAroundScreen();
}


//-----------------------------------------------------------------------------------------------
void Beetle::Render() const
{
	// Create a copy of the beetle's vertex array to transform
	Vertex tempBeetleWorldVerts[NUM_BEETLE_VERTS];
	for ( int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++ vertIndex )
	{
		tempBeetleWorldVerts[vertIndex] = m_vertexArray[vertIndex];
	}
	// Transform the copy to world space and render the vertexes
	TransformVertexArrayXY3D( NUM_BEETLE_VERTS, tempBeetleWorldVerts, 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( NUM_BEETLE_VERTS, tempBeetleWorldVerts );
}


//-----------------------------------------------------------------------------------------------
void Beetle::InitializeVertexArray()
{
	m_vertexArray = new Vertex[NUM_BEETLE_VERTS];
	
	m_vertexArray[0].m_position = Vec3( -2.f, 0.f, 0.f );
	m_vertexArray[1].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_vertexArray[2].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[0].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[1].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[2].m_color = Rgba8( 153, 255, 51 );

	m_vertexArray[3].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[4].m_position = Vec3( 0.5f, 0.5f, 0.f );
	m_vertexArray[5].m_position = Vec3( 0.f, 2.f, 0.f );
	m_vertexArray[3].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[4].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[5].m_color = Rgba8( 153, 255, 51 );

	m_vertexArray[6].m_position = Vec3( 0.5f, 0.5f, 0.f );
	m_vertexArray[7].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[8].m_position = Vec3( 2.f, 0.f, 0.f );
	m_vertexArray[6].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[7].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[8].m_color = Rgba8( 153, 255, 51 );

	m_vertexArray[9].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[10].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_vertexArray[11].m_position = Vec3( 0.f, -2.f, 0.f );
	m_vertexArray[9].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[10].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[11].m_color = Rgba8( 153, 255, 51 );

	m_vertexArray[12].m_position = Vec3( -0.5f, -0.5f, 0.f );
	m_vertexArray[13].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[14].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[12].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[13].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[14].m_color = Rgba8( 153, 255, 51 );

	m_vertexArray[15].m_position = Vec3( 0.5f, -0.5f, 0.f );
	m_vertexArray[16].m_position = Vec3( 0.5f, 0.5f, 0.f );
	m_vertexArray[17].m_position = Vec3( -0.5f, 0.5f, 0.f );
	m_vertexArray[15].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[16].m_color = Rgba8( 153, 255, 51 );
	m_vertexArray[17].m_color = Rgba8( 153, 255, 51 );
};