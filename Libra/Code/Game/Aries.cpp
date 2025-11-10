#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Aries::Aries( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_angularVelocityDegreesPerSecond = ARIES_TURN_SPEED_DEGREES_PER_SECOND;
	m_velocity = ARIES_MOVE_SPEED_TILES_PER_SECOND * GetForwardNormal();

	m_faction = ENTITY_FACTION_EVIL;
	m_physicsRadius = ARIES_PHYSICS_RADIUS;
	m_cosmeticRadius = ARIES_COSMETIC_RADIUS;
	m_health = ARIES_HEALTH;

	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Aries::~Aries() = default;


//-----------------------------------------------------------------------------------------------
void Aries::Update( float deltaSeconds )
{
	float distanceToPlayer = ( g_game->m_player->m_position - m_position ).GetLength();
	bool hasLineOfSightToPlayer = g_game->m_currentMap->HasLineOfSight( m_position, g_game->m_player->m_position, 0.1f );
	if ( distanceToPlayer <= VISIBLE_RANGE_RADIUS && hasLineOfSightToPlayer )
	{
		m_targetPosition = g_game->m_player->m_position;
	}

	float distanceToTarget = ( m_targetPosition - m_position ).GetLength();
	if ( distanceToTarget <= m_physicsRadius )
	{
		m_targetPosition = Vec2::ZERO;
	}

	if ( m_targetPosition != Vec2::ZERO )
	{
		MoveTowardTargetPosition( deltaSeconds );
	}
	else
	{
		RandomNumberGenerator rng;
		m_timeSinceLastTurn += deltaSeconds;
		if ( m_timeSinceLastTurn >= ARIES_TURN_COOLDOWN_SECONDS )
		{
			float randomAngle = rng.RollRandomFloatInRange( 0.f, 360.f );
			m_targetPosition = m_position + Vec2::MakeFromPolarDegrees( randomAngle, 20.f );
			m_timeSinceLastTurn = 0.f;
		}
		MoveTowardTargetPosition( deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Aries::Render() const
{
	if ( m_isDead )
	{
		return;
	}

	std::vector<Vertex> verts = m_vertexArray;
	Texture* ariesTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyAries.png" );
	g_engine->m_renderer->BindTexture( ariesTexture );
	TransformVertexArrayXY3D( ( int ) verts.size(), verts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Aries::TakeDamage( int damage )
{
	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Aries::Die()
{
	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Aries::InitializeVertexArray()
{
	// Body
	AABB2 bodyAABB2 = AABB2( -m_cosmeticRadius, -m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius );
	AddVertsForAABB2D( m_vertexArray, bodyAABB2, Rgba8::WHITE );
}


//-----------------------------------------------------------------------------------------------
void Aries::MoveTowardTargetPosition( float deltaSeconds )
{
	// Turn toward target position
	float targetOrientationDegrees = ( m_targetPosition - m_position ).GetOrientationDegrees();
	float angleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, targetOrientationDegrees );
	if ( fabsf( angleDiff ) <= 45.f )
	{
		m_velocity = ARIES_MOVE_SPEED_TILES_PER_SECOND * GetForwardNormal();
	}
	else
	{
		m_velocity = Vec2::ZERO;
	}
	float maxDeltaThisFrame = m_angularVelocityDegreesPerSecond * deltaSeconds;
	if ( fabsf( angleDiff ) <= maxDeltaThisFrame )
	{
		m_orientationDegrees = targetOrientationDegrees;
	}
	else
	{
		if ( angleDiff > 0.f )
		{
			m_orientationDegrees += maxDeltaThisFrame;
		}
		else
		{
			m_orientationDegrees -= maxDeltaThisFrame;
		}
	}
	m_position += m_velocity * deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void Aries::ReflectBullet( Bullet& bullet )
{
	Vec2 toBullet = bullet.m_position - m_position;
	Vec2 forward = GetForwardNormal();
	Vec2 bulletDir = bullet.m_velocity.GetNormalized();
	float angleToForward = GetShortestAngularDispDegrees( forward.GetOrientationDegrees(), ( bullet.m_position - m_position ).GetOrientationDegrees() );

	if ( fabsf( angleToForward ) <= 45.f )
	{
		Vec2 hitDir = toBullet.GetNormalized();
		float hitAngle = GetShortestAngularDispDegrees( forward.GetOrientationDegrees(), hitDir.GetOrientationDegrees() );

		Vec2 shieldNormal = Vec2::MakeFromPolarDegrees( forward.GetOrientationDegrees() + hitAngle, 1.f );
		bullet.m_velocity = bullet.m_velocity.GetReflected( shieldNormal );

		bullet.m_orientationDegrees = bullet.m_velocity.GetOrientationDegrees();
	}
}