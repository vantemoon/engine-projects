#include "Game/Scorpio.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Scorpio::Scorpio( Vec2 startingPosition, float orientationDegrees )
	: Entity( startingPosition, orientationDegrees )
{
	m_angularVelocityDegreesPerSecond = SCORPIO_TURN_SPEED_DEGREES_PER_SECOND;

	m_isPushedByWalls = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_faction = ENTITY_FACTION_EVIL;

	m_physicsRadius = SCORPIO_PHYSICS_RADIUS;
	m_cosmeticRadius = SCORPIO_COSMETIC_RADIUS;
	m_health = SCORPIO_HEALTH;

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
	if ( distanceToPlayer <= VISIBLE_RANGE_RADIUS && hasLineOfSightToPlayer )
	{
		Vec2 toPlayer = g_game->m_player->m_position - m_position;
		m_targetOrientationDegrees = toPlayer.GetOrientationDegrees();

		float angleDiff = GetShortestAngularDispDegrees( m_orientationDegrees, m_targetOrientationDegrees );
		if ( fabsf( angleDiff ) < 5.f )
		{
			m_timeSinceLastFire += deltaSeconds;
			if ( m_timeSinceLastFire >= SCORPIO_FIRE_COOLDOWN_SECONDS )
			{
				FireProjectile();
				m_timeSinceLastFire = 0.f;
			}
		}
	}
	else
	{
		m_angularVelocityDegreesPerSecond = SCORPIO_TURN_SPEED_DEGREES_PER_SECOND;
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
	Texture* scorpioBaseTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	g_engine->m_renderer->BindTexture( scorpioBaseTexture );
	TransformVertexArrayXY3D( ( int ) baseVerts.size(), baseVerts.data(), 1.f, 0.f, m_position );
	g_engine->m_renderer->DrawVertexArray( baseVerts );

	// Top (rotated)
	std::vector<Vertex> topVerts = m_scorpioTopVertexArray;
	Texture* scorpioTopTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/EnemyCannon.png" );
	g_engine->m_renderer->BindTexture( scorpioTopTexture );
	TransformVertexArrayXY3D( ( int ) topVerts.size(), topVerts.data(), 1.f, m_orientationDegrees, m_position );
	g_engine->m_renderer->DrawVertexArray( topVerts );

	// Laser line
	Vec2 laserStart = m_position + Vec2::MakeFromPolarDegrees( m_orientationDegrees, 5.5f );
	Vec2 forwardDirection = Vec2::MakeFromPolarDegrees( m_orientationDegrees, 1.f );
	RaycastResult2D raycastResult = g_game->m_currentMap->RaycastVsTiles( m_position, forwardDirection, SCORPIO_MAX_RANGE, 0.1f );
	Vec2 impactPos;
	if ( raycastResult.m_didImpact )
	{
		impactPos = raycastResult.m_impactPos;
	}
	else
	{
		impactPos = laserStart + forwardDirection * SCORPIO_MAX_RANGE;
	}
	float distanceToImpact = ( impactPos - laserStart ).GetLength();
	float laserAlpha = 1.f - ( distanceToImpact / SCORPIO_MAX_RANGE );
	Rgba8 laserColor = Rgba8( 255, 0, 0, ( unsigned char ) ( laserAlpha * 255.f ) );
	float laserThickness = 0.5f;
	g_engine->m_renderer->BindTexture( nullptr );
	DebugDrawLine( laserStart, impactPos, laserThickness, Rgba8::RED, laserColor );


	g_engine->m_renderer->BindTexture( nullptr );
}


//-----------------------------------------------------------------------------------------------
void Scorpio::TakeDamage( int damage )
{
	Entity::TakeDamage( damage );
}


//-----------------------------------------------------------------------------------------------
void Scorpio::Die()
{
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
	Vec2 muzzleOffset = Vec2::MakeFromPolarDegrees( m_orientationDegrees, m_physicsRadius + 0.5f );
	Vec2 projectileSpawnPosition = m_position + muzzleOffset;
	Entity* newBullet = g_game->m_currentMap->SpawnNewEntity( ENTITY_TYPE_EVIL_BOLT, projectileSpawnPosition, m_orientationDegrees );
	if ( newBullet != nullptr )
		g_game->m_currentMap->AddEntityToMap( *newBullet, ENTITY_TYPE_EVIL_BOLT );
}