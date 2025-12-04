#include "Game/Explosion.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
Explosion::Explosion( Vec2 const& position, float orientationDegrees, float duration, float size )
	: Entity( position, orientationDegrees )
	, m_duration( duration )
	, m_size( size )
{
	m_isPushedByWalls    = g_gameConfigBlackboard.GetValue( "explosionIsPushedByWalls", false );
	m_isPushedByEntities = g_gameConfigBlackboard.GetValue( "explosionIsPushedByEntities", false );
	m_doesPushEntities   = g_gameConfigBlackboard.GetValue( "explosionDoesPushEntities", false );
	m_isHitByBullets     = g_gameConfigBlackboard.GetValue( "explosionIsHitByBullets", false );

	m_faction = ENTITY_FACTION_NEUTRAL;

	m_explosionSpriteSheet = new SpriteSheet( *g_game->m_explosionTexture, IntVec2( 5, 5 ) );
	m_explosionAnimDef = new SpriteAnimDefinition(
		*m_explosionSpriteSheet,
		0,
		m_explosionSpriteSheet->GetNumSprites() - 1,
		30.f,
		ONCE
	);

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Explosion::~Explosion()
{
	delete m_explosionAnimDef;
	m_explosionAnimDef = nullptr;

	delete m_explosionSpriteSheet;
	m_explosionSpriteSheet = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Explosion::Update( float deltaSeconds )
{
	m_age += deltaSeconds;
	if ( m_age >= m_duration )
	{
		Entity::Die();
	}
}


//-----------------------------------------------------------------------------------------------
void Explosion::Render() const
{
	SpriteDefinition currentSpriteDef = m_explosionAnimDef->GetSpriteDefAtTime( m_age );

	Vec2 uvMins;
	Vec2 uvMaxs;
	currentSpriteDef.GetUVs( uvMins, uvMaxs );

	std::vector<Vertex> verts;
	AABB2 localBounds( -m_size, -m_size, m_size, m_size );
	AddVertsForAABB2D( verts, localBounds, Rgba8::WHITE, uvMins, uvMaxs );

	TransformVertexArrayXY3D(
		( int ) verts.size(),
		verts.data(),
		1.0f,
		m_orientationDegrees,
		m_position );

	g_engine->m_renderer->SetBlendMode( BlendMode::ADDITIVE );
	g_engine->m_renderer->BindTexture( &m_explosionSpriteSheet->GetTexture() );
	g_engine->m_renderer->DrawVertexArray( ( int ) verts.size(), verts.data() );
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
}


//-----------------------------------------------------------------------------------------------
void Explosion::InitializeVertexArray()
{
	AABB2 explosionAABB2 = AABB2( -m_size, -m_size, m_size, m_size );
	AddVertsForAABB2D( m_vertexArray, explosionAABB2, Rgba8::WHITE );
}