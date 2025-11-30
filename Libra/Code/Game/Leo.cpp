#include "Game/Leo.hpp"
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
Leo::Leo( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_angularVelocityDegreesPerSecond = g_gameConfigBlackboard.GetValue( "leoTurnSpeed", 90.f );
	m_velocity = g_gameConfigBlackboard.GetValue( "leoMoveSpeed", 6.f ) * GetForwardNormal();

	m_physicsRadius = g_gameConfigBlackboard.GetValue( "leoPhysicsRadius", 4.f );
	m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "leoCosmeticRadius", 6.f );
	m_health = g_gameConfigBlackboard.GetValue( "leoMaxHealth", 10 );

	m_isPushedByWalls = g_gameConfigBlackboard.GetValue( "leoIsPushedByWalls", true );
	m_isPushedByEntities = g_gameConfigBlackboard.GetValue( "leoIsPushedByEntities", true );
	m_doesPushEntities = g_gameConfigBlackboard.GetValue( "leoDoesPushEntities", true );
	m_isHitByBullets = g_gameConfigBlackboard.GetValue( "leoIsHitByBullets", true );

	m_faction = ENTITY_FACTION_EVIL;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Leo::~Leo() = default;


//-----------------------------------------------------------------------------------------------
void Leo::Update( float deltaSeconds )
{
	// Check if player is alive
	bool playerIsAlive = g_game->m_player->IsAlive();

	float distanceToPlayer = ( g_game->m_player->m_position - m_position ).GetLength();
	bool hasLineOfSightToPlayer = g_game->m_currentMap->HasLineOfSight( m_position, g_game->m_player->m_position, 0.1f );

	// Only pursue player if alive
	float sightRadius = g_gameConfigBlackboard.GetValue( "leoSightRadius", 125.f );
	if ( playerIsAlive && distanceToPlayer <= sightRadius && hasLineOfSightToPlayer )
	{
		m_targetPosition = g_game->m_player->m_position;
		float algleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, ( m_targetPosition - m_position ).GetOrientationDegrees() );
		if ( fabsf( algleDiff ) < 5.f )
		{
			m_timeSinceLastFire += deltaSeconds;
			if ( m_timeSinceLastFire >= g_gameConfigBlackboard.GetValue( "leoFireCooldown", 1.2f ) )
			{
				FireProjectile();
				m_timeSinceLastFire = 0.f;
			}
		}
	}
	else if ( !playerIsAlive )
	{
		// If player is dead, clear target position to trigger idle behavior
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

		if ( m_timeSinceLastTurn >= g_gameConfigBlackboard.GetValue( "leoTurnCooldown", 1.5f ) || needNewTarget )
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
void Leo::Render() const
{
	if ( m_isDead )
	{
		return;
	}
	
	std::vector<Vertex> verts = m_vertexArray;
	g_engine->m_renderer->BindTexture( g_game->m_leoTexture );
	TransformVertexArrayXY3D( ( int ) verts.size(), verts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Leo::TakeDamage( int damage )
{
	g_engine->m_audioSystem->StartSound( g_game->m_enemyHitSoundID );

	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Leo::Die()
{
	g_engine->m_audioSystem->StartSound( g_game->m_enemyDeathSoundID );

	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Leo::InitializeVertexArray()
{
	// Body
	AABB2 bodyAABB2 = AABB2( -m_cosmeticRadius, -m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius );
	AddVertsForAABB2D( m_vertexArray, bodyAABB2, Rgba8::WHITE );
}


//-----------------------------------------------------------------------------------------------
void Leo::MoveTowardTargetPosition( float deltaSeconds )
{
	// Turn toward target position
	float targetOrientationDegrees = ( m_targetPosition - m_position ).GetOrientationDegrees();
	float angleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, targetOrientationDegrees );
	if ( fabsf( angleDiff ) <= 45.f )
	{
		m_velocity = g_gameConfigBlackboard.GetValue( "leoMoveSpeed", 6.f ) * GetForwardNormal();
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
void Leo::FireProjectile()
{
	Vec2 muzzleOffset = Vec2::MakeFromPolarDegrees( m_orientationDegrees, m_physicsRadius + 0.5f );
	Vec2 projectileSpawnPosition = m_position + muzzleOffset;
	Entity* newBullet = g_game->m_currentMap->SpawnNewEntity( ENTITY_TYPE_EVIL_BULLET, projectileSpawnPosition, m_orientationDegrees );
	if ( newBullet != nullptr )
		g_game->m_currentMap->AddEntityToMap( *newBullet, ENTITY_TYPE_EVIL_BULLET, ENTITY_FACTION_EVIL );

	g_engine->m_audioSystem->StartSound( g_game->m_enemyShootSoundID );
}