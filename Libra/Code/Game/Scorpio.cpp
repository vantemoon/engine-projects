#include "Game/Scorpio.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Scorpio::Scorpio( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_physicsRadius = g_gameConfigBlackboard.GetValue( "scorpioPhysicsRadius", 4.f );
	m_cosmeticRadius = g_gameConfigBlackboard.GetValue( "scorpioCosmeticRadius", 6.f );
	m_health = g_gameConfigBlackboard.GetValue( "scorpioMaxHealth", 10 );
	m_angularVelocityDegreesPerSecond = g_gameConfigBlackboard.GetValue( "scorpioTurnSpeed", 60.f );

	m_isPushedByWalls = g_gameConfigBlackboard.GetValue( "scorpioIsPushedByWalls", true );
	m_isPushedByEntities = g_gameConfigBlackboard.GetValue( "scorpioIsPushedByEntities", false );
	m_doesPushEntities = g_gameConfigBlackboard.GetValue( "scorpioDoesPushEntities", true );
	m_isHitByBullets = g_gameConfigBlackboard.GetValue( "scorpioIsHitByBullets", true );
	m_canSwim = g_gameConfigBlackboard.GetValue( "scorpioCanSwim", false );

	m_faction = ENTITY_FACTION_EVIL;

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Scorpio::~Scorpio() = default;


//-----------------------------------------------------------------------------------------------
void Scorpio::Update( float deltaSeconds )
{
	if ( m_isDead )
	{
		return;
	}

	float distanceToPlayer = ( g_game->m_player->m_position - m_position ).GetLength();
	bool hasLineOfSightToPlayer = g_game->m_currentMap->HasLineOfSight( m_position, g_game->m_player->m_position, 0.1f );

	float sightRadius = g_gameConfigBlackboard.GetValue( "scorpioSightRadius", 125.f );
	if ( g_game->m_player->IsAlive() && distanceToPlayer <= sightRadius && hasLineOfSightToPlayer )
	{
		Vec2 toPlayer = g_game->m_player->m_position - m_position;
		m_targetOrientationDegrees = toPlayer.GetOrientationDegrees();

		float angleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, m_targetOrientationDegrees );
		if ( fabsf( angleDiff ) < 5.f )
		{
			m_timeSinceLastFire += deltaSeconds;
			if ( m_timeSinceLastFire >= g_gameConfigBlackboard.GetValue( "scorpioFireCooldown", 0.3f ) )
			{
				FireProjectile();
				m_timeSinceLastFire = 0.f;
			}
		}
	}
	else
	{
		m_angularVelocityDegreesPerSecond = g_gameConfigBlackboard.GetValue( "scorpioTurnSpeed", 60.f );
		m_targetOrientationDegrees += m_angularVelocityDegreesPerSecond * deltaSeconds;
	}

	TurnTowardTargetOrientation( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Scorpio::Render() const
{
	if ( m_isDead )
	{
		return;
	}

	// Base (not rotated)
	std::vector<Vertex> baseVerts = m_scorpioBaseVertexArray;
	g_engine->m_renderer->BindTexture( g_game->m_scorpioBaseTexture );
	TransformVertexArrayXY3D( ( int ) baseVerts.size(), baseVerts.data(), 1.f, 0.f, m_position );
	g_engine->m_renderer->DrawVertexArray( baseVerts );

	// Top (rotated)
	std::vector<Vertex> topVerts = m_scorpioTopVertexArray;
	g_engine->m_renderer->BindTexture( g_game->m_scorpioTurretTexture );
	TransformVertexArrayXY3D( ( int ) topVerts.size(), topVerts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( topVerts );

	// Laser line
	float scorpioMaxRange = g_gameConfigBlackboard.GetValue( "scorpioMaxRange", 125.f );
	Vec2 laserStart = m_position + Vec2::MakeFromPolarDegrees( m_orientationDegrees, 5.5f );
	Vec2 forwardDirection = Vec2::MakeFromPolarDegrees( m_orientationDegrees, 1.f );
	RaycastResult2D raycastResult = g_game->m_currentMap->RaycastVsTiles( m_position, forwardDirection, scorpioMaxRange, 0.1f );
	Vec2 impactPos;
	if ( raycastResult.m_didImpact )
	{
		impactPos = raycastResult.m_impactPos;
	}
	else
	{
		impactPos = laserStart + forwardDirection * scorpioMaxRange;
	}
	float distanceToImpact = ( impactPos - laserStart ).GetLength();
	float laserAlpha = 1.f - ( distanceToImpact / scorpioMaxRange );
	Rgba8 laserColor = Rgba8( 255, 0, 0, ( unsigned char ) ( laserAlpha * 255.f ) );
	float laserThickness = 0.5f;
	g_engine->m_renderer->BindTexture( nullptr );
	DebugDrawLine( laserStart, impactPos, laserThickness, Rgba8::RED, laserColor );


	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Scorpio::TakeDamage( int damage )
{
	g_engine->m_audioSystem->StartSound( g_game->m_enemyHitSoundID );

	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Scorpio::Die()
{
	g_engine->m_audioSystem->StartSound( g_game->m_enemyDeathSoundID );
	g_game->m_currentMap->SpawnExplosionAtPosition( m_position, 1.f, 8.f );

	Entity::Die();
}


//-----------------------------------------------------------------------------------------------
void Scorpio::InitializeVertexArray()
{
	// Base (not rotated)
	AABB2 bodyAABB2 = AABB2( -m_cosmeticRadius, -m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius );
	AddVertsForAABB2D( m_scorpioBaseVertexArray, bodyAABB2, Rgba8::WHITE );

	// Top (rotated)
	AABB2 topAABB2 = AABB2( -m_cosmeticRadius, -m_cosmeticRadius, m_cosmeticRadius, m_cosmeticRadius );
	AddVertsForAABB2D( m_scorpioTopVertexArray, topAABB2, Rgba8::WHITE );
}


//-----------------------------------------------------------------------------------------------
void Scorpio::TurnTowardTargetOrientation( float deltaSeconds )
{
	float angleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, m_targetOrientationDegrees );
	float maxDeltaThisFrame = m_angularVelocityDegreesPerSecond * deltaSeconds;
	if ( fabsf( angleDiff ) <= maxDeltaThisFrame )
	{
		m_orientationDegrees = m_targetOrientationDegrees;
	}
	else
	{
		float turnDirection = ( angleDiff > 0.f ) ? 1.f : -1.f;
		m_orientationDegrees += turnDirection * maxDeltaThisFrame;
	}
}


//-----------------------------------------------------------------------------------------------
void Scorpio::FireProjectile()
{
	Vec2 muzzleOffset = Vec2::MakeFromPolarDegrees( m_orientationDegrees, m_physicsRadius + 0.3f );
	Vec2 projectileSpawnPosition = m_position + muzzleOffset;
	Entity* newBullet = g_game->m_currentMap->SpawnNewEntity( ENTITY_TYPE_EVIL_BOLT, projectileSpawnPosition, m_orientationDegrees );
	if ( newBullet != nullptr )
		g_game->m_currentMap->AddEntityToMap( *newBullet, ENTITY_TYPE_EVIL_BOLT, ENTITY_FACTION_EVIL );

	g_engine->m_audioSystem->StartSound( g_game->m_enemyShootSoundID );
}