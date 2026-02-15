#include "Game/ImpactWave.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
ImpactWave::ImpactWave( Game* game, Vec2 const& center, float lifeSeconds, float startRadius, float endRadius, float startThickness, float endThickness,
						Rgba8 const& startColour, Rgba8 const& endColour )
	: Entity( game, center )
{
	m_lifeSeconds = lifeSeconds;
	m_startRadius = startRadius;
	m_endRadius = endRadius;
	m_startThick = startThickness;
	m_endThick = endThickness;
	m_startColour = startColour;
	m_endColour = endColour;

	m_physicsRadius = 0.f;
	m_isDead = false;
}


//-----------------------------------------------------------------------------------------------
void ImpactWave::Update()
{
	float deltaSeconds = ( float ) m_game->m_gameClock->GetDeltaSeconds();
	m_ageSeconds += deltaSeconds;
	if ( m_ageSeconds >= m_lifeSeconds )
	{
		m_isDead = true;
		m_isGarbage = true;
	}

	Entity::Update();
}


//-----------------------------------------------------------------------------------------------
void ImpactWave::Render() const
{
	if ( m_isDead ) return;

	float lifeFraction = GetClampedZeroToOne( m_ageSeconds / m_lifeSeconds );
	float ease = lifeFraction * ( 2.f - lifeFraction );

	float radius = Interpolate( m_startRadius, m_endRadius, ease );
	float thick = Interpolate( m_startThick, m_endThick, ease );
	Rgba8 innerColor;
	innerColor.r = ( unsigned char ) Interpolate( ( float ) m_startColour.r, ( float ) m_endColour.r, lifeFraction );
	innerColor.g = ( unsigned char ) Interpolate( ( float ) m_startColour.g, ( float ) m_endColour.g, lifeFraction );
	innerColor.b = ( unsigned char ) Interpolate( ( float ) m_startColour.b, ( float ) m_endColour.b, lifeFraction );
	innerColor.a = ( unsigned char ) Interpolate( ( float ) m_startColour.a, ( float ) m_endColour.a, lifeFraction );
	Rgba8 outerColor = innerColor;
	outerColor.a = 0;


	float innerR = radius - thick * 0.5f;
	float outerR = radius + thick * 0.5f;
	if ( innerR < 0.f ) innerR = 0.f;

	const int   segments = NUM_SEGMENTS;
	const int   maxVerts = segments * 6;
	Vertex      verts[maxVerts];
	int         vertexIndex = 0;

	Vec2 prevInner = m_position + Vec2( CosDegrees( 0.f ) * innerR, SinDegrees( 0.f ) * innerR );
	Vec2 prevOuter = m_position + Vec2( CosDegrees( 0.f ) * outerR, SinDegrees( 0.f ) * outerR );

	for ( int s = 1; s <= segments; ++s )
	{
		float deg = 360.f * ( float ) s / ( float ) segments;
		Vec2 currInner = m_position + Vec2( CosDegrees( deg ) * innerR, SinDegrees( deg ) * innerR );
		Vec2 currOuter = m_position + Vec2( CosDegrees( deg ) * outerR, SinDegrees( deg ) * outerR );

		verts[vertexIndex++] = Vertex( Vec3( prevInner.x, prevInner.y, 0.f ), innerColor, Vec2( 0, 0 ) );
		verts[vertexIndex++] = Vertex( Vec3( prevOuter.x, prevOuter.y, 0.f ), outerColor, Vec2( 0, 0 ) );
		verts[vertexIndex++] = Vertex( Vec3( currOuter.x, currOuter.y, 0.f ), outerColor, Vec2( 0, 0 ) );

		verts[vertexIndex++] = Vertex( Vec3( prevInner.x, prevInner.y, 0.f ), innerColor, Vec2( 0, 0 ) );
		verts[vertexIndex++] = Vertex( Vec3( currOuter.x, currOuter.y, 0.f ), outerColor, Vec2( 0, 0 ) );
		verts[vertexIndex++] = Vertex( Vec3( currInner.x, currInner.y, 0.f ), innerColor, Vec2( 0, 0 ) );

		prevInner = currInner;
		prevOuter = currOuter;
	}

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( vertexIndex, verts );
}