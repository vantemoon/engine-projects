#include "Game/Debris.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Debris::Debris( Game* game, Vec2 const& startingPosition, Vec2 const& startingVelocity, 
                float startingAngularVelocity, Rgba8 const& color, float minRadius, float maxRadius )
	: Entity( game, startingPosition )
{
	RandomNumberGenerator rng;
	m_numOfSides = rng.RollRandomIntInRange( DEBRIS_MIN_NUM_OF_SIDES, DEBRIS_MAX_NUM_OF_SIDES );
	m_numOfVerts = m_numOfSides * 3;
	m_minRadius = minRadius;
	m_maxRadius = maxRadius;
    m_color = color;
	m_velocity = startingVelocity;
	m_angularVelocityDegreesPerSecond = startingAngularVelocity;
	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Debris::~Debris()
{
	delete[] m_vertexArray;
	m_vertexArray = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Debris::Update( float deltaSeconds )
{
	m_ageSeconds += deltaSeconds;
	if ( m_ageSeconds >= DEBRIS_LIFETIME_SECONDS || IsOffScreen() )
	{
		m_isDead = true;
		m_isGarbage = true;
	}

	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Debris::Render() const
{
    Vertex tempWaspWorldVerts[DEBRIS_MAX_NUM_OF_VERTS];
    for (int vertIndex = 0; vertIndex < m_numOfVerts; ++vertIndex)
    {
        tempWaspWorldVerts[vertIndex] = m_vertexArray[vertIndex];
		tempWaspWorldVerts[vertIndex].m_color.a = static_cast<unsigned char>(RangeMapClamped(m_ageSeconds, 0.f, DEBRIS_LIFETIME_SECONDS, 255.f, 0.f));
    }
    // Transform the copy to world space and render the vertexes
    TransformVertexArrayXY3D(m_numOfVerts, tempWaspWorldVerts, 1.f, m_orientationDegrees, m_position);
    g_engine->m_renderer->DrawVertexArray(m_numOfVerts, tempWaspWorldVerts);
}


//-----------------------------------------------------------------------------------------------
void Debris::InitializeVertexArray()
{
	m_vertexArray = new Vertex[DEBRIS_MAX_NUM_OF_SIDES * 3];

    RandomNumberGenerator rng;
    float radii[DEBRIS_MAX_NUM_OF_SIDES];
    for ( int vertIndex = 0; vertIndex < m_numOfSides; ++vertIndex )
    {
        radii[vertIndex] = rng.RollRandomFloatInRange( m_minRadius, m_maxRadius );
    }

    float rotationDegrees = 360.f / ( float ) m_numOfSides;
    for ( int sideIndex = 0; sideIndex < m_numOfSides; ++ sideIndex )
    {
        Vec3 vertexPos1 = Vec3( 0.f, 0.f, 0 );
        Vec3 vertexPos2 = Vec3( radii[sideIndex], 0.f, 0 );
		Vec3 vertexPos3 = Vec3( radii[( sideIndex + 1 ) % m_numOfSides], 0.f, 0 );
        TransformPositionXY3D( vertexPos2, 1.f, rotationDegrees * ( float ) sideIndex, Vec2( 0.f, 0.f ) );
        TransformPositionXY3D( vertexPos3, 1.f, rotationDegrees * ( float )( sideIndex + 1 ), Vec2( 0.f, 0.f ) );
        
        // Triangle
        m_vertexArray[sideIndex * 3 + 0] = Vertex( vertexPos1, m_color, Vec2() );
        m_vertexArray[sideIndex * 3 + 1] = Vertex( vertexPos2, m_color, Vec2() );
		m_vertexArray[sideIndex * 3 + 2] = Vertex( vertexPos3, m_color, Vec2() );
    }
}