#include "Game/Leo.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Leo::Leo( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_angularVelocityDegreesPerSecond = LEO_TURN_SPEED_DEGREES_PER_SECOND;
	m_velocity = LEO_MOVE_SPEED_TILES_PER_SECOND * GetForwardNormal();

	m_faction = ENTITY_FACTION_EVIL;
	m_physicsRadius = LEO_PHYSICS_RADIUS;
	m_cosmeticRadius = LEO_COSMETIC_RADIUS;
	m_health = LEO_HEALTH;

	m_isPushedByWalls = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

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
	if ( playerIsAlive && distanceToPlayer <= VISIBLE_RANGE_RADIUS && hasLineOfSightToPlayer )
	{
		m_targetPosition = g_game->m_player->m_position;
		float algleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, ( m_targetPosition - m_position ).GetOrientationDegrees() );
		if ( fabsf( algleDiff ) < 5.f )
		{
			m_timeSinceLastFire += deltaSeconds;
			if ( m_timeSinceLastFire >= LEO_FIRE_COOLDOWN_SECONDS )
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

		if ( m_timeSinceLastTurn >= LEO_TURN_COOLDOWN_SECONDS || needNewTarget )
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
	Texture* leoTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank4.png" );
	g_engine->m_renderer->BindTexture( leoTexture );
	TransformVertexArrayXY3D( ( int ) verts.size(), verts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Leo::TakeDamage( int damage )
{
	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Leo::Die()
{
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
		m_velocity = LEO_MOVE_SPEED_TILES_PER_SECOND * GetForwardNormal();
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
}