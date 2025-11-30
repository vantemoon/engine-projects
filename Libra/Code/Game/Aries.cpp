#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Aries::Aries( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_angularVelocityDegreesPerSecond = g_gameConfigBlackboard.GetValue( "ariesTurnSpeed", 60.f );
	m_velocity = g_gameConfigBlackboard.GetValue( "ariesMoveSpeed", 6.f ) * GetForwardNormal();

	m_physicsRadius = g_gameConfigBlackboard.GetValue( "ariesPhysicsRadius", 4.f );
	m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "ariesCosmeticRadius", 6.f );
	m_health = g_gameConfigBlackboard.GetValue( "ariesMaxHealth", 10 );

	m_isPushedByWalls = g_gameConfigBlackboard.GetValue( "ariesIsPushedByWalls", true );
	m_isPushedByEntities = g_gameConfigBlackboard.GetValue( "ariesIsPushedByEntities", true );
	m_doesPushEntities = g_gameConfigBlackboard.GetValue( "ariesDoesPushEntities", true );
	m_isHitByBullets = g_gameConfigBlackboard.GetValue( "ariesIsHitByBullets", true );

	m_faction = ENTITY_FACTION_EVIL;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Aries::~Aries() = default;


//-----------------------------------------------------------------------------------------------
void Aries::Update( float deltaSeconds )
{
	bool playerIsAlive = g_game->m_player->IsAlive();
	float distanceToPlayer = ( g_game->m_player->m_position - m_position ).GetLength();
	bool hasLineOfSightToPlayer = g_game->m_currentMap->HasLineOfSight( m_position, g_game->m_player->m_position, 0.1f );

	float sightRadius = g_gameConfigBlackboard.GetValue( "ariesSightRadius", 125.f );
	if ( playerIsAlive && distanceToPlayer <= sightRadius && hasLineOfSightToPlayer )
	{
		m_targetPosition = g_game->m_player->m_position;
	}
	else if ( !playerIsAlive )
	{
		m_targetPosition = Vec2::ZERO;
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
		bool needNewTarget = false;

		// Check if current random target is unreachable or reached
		if ( m_targetPosition == Vec2::ZERO ||
			!g_game->m_currentMap->HasLineOfSight( m_position, m_targetPosition, 0.1f ) ||
			( m_targetPosition - m_position ).GetLength() <= m_physicsRadius )
		{
			needNewTarget = true;
		}

		if ( m_timeSinceLastTurn >= g_gameConfigBlackboard.GetValue( "ariesTurnCooldown", 1.5f ) || needNewTarget )
		{
			// Try to find a reachable random target
			for ( int attempts = 0; attempts < 5; ++attempts )
			{
				float randomAngle = rng.RollRandomFloatInRange( 0.f, 360.f );
				Vec2 candidate = m_position + Vec2::MakeFromPolarDegrees( randomAngle, 20.f );
				if ( g_game->m_currentMap->HasLineOfSight( m_position, candidate, 0.1f ) )
				{
					m_targetPosition = candidate;
					break;
				}
			}
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
	g_engine->m_renderer->BindTexture( g_game->m_ariesTexture );
	TransformVertexArrayXY3D( ( int ) verts.size(), verts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Aries::TakeDamage( int damage )
{
	g_engine->m_audioSystem->StartSound( g_game->m_enemyHitSoundID );

	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Aries::Die()
{
	g_engine->m_audioSystem->StartSound( g_game->m_enemyDeathSoundID );

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
		m_velocity = g_gameConfigBlackboard.GetValue( "ariesMoveSpeed", 6.f ) * GetForwardNormal();
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