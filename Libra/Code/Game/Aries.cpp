#include "Game/Aries.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
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
	m_canSwim = g_gameConfigBlackboard.GetValue( "ariesCanSwim", true );

	m_faction = ENTITY_FACTION_EVIL;

	int randomSeed = m_rng.RollRandomIntInRange( 0, 10000 ) + static_cast<int>( GetCurrentTimeSeconds() ) * 12345;
	m_rng.SetSeed( randomSeed );

	InitializeVertexArray();
}


//-----------------------------------------------------------------------------------------------
Aries::~Aries() = default;


//-----------------------------------------------------------------------------------------------
void Aries::Update( float deltaSeconds )
{
	bool playerIsAlive = g_game->m_player->IsAlive();
	float distanceToPlayer = ( g_game->m_player->m_position - m_position ).GetLength();
	bool hasLOS = g_game->m_currentMap->HasLineOfSight( m_position, g_game->m_player->m_position, 0.1f );
	float sightRadius = g_gameConfigBlackboard.GetValue( "ariesSightRadius", 125.f );
	bool playerVisible = ( playerIsAlive && distanceToPlayer <= sightRadius && hasLOS );

	if ( playerVisible )
	{
		m_lastSeenPlayerPosition = g_game->m_player->m_position;
		m_goalPosition = m_lastSeenPlayerPosition;
	}
	else if ( !playerIsAlive )
	{
		m_lastSeenPlayerPosition = Vec2::ZERO;
		m_waypointPosition = Vec2::ZERO;
		m_wanderGoalPosition = Vec2::ZERO;
		m_wanderGoalTile = IntVec2( -1, -1 );
		m_goalPosition = Vec2::ZERO;

		if ( m_dijkstraMap != nullptr )
		{
			delete m_dijkstraMap;
			m_dijkstraMap = nullptr;
		}
	}

	Vec2 movementTarget = Vec2::ZERO;

	if ( playerVisible )
	{
		if ( IsPlayerInAdjacentTile() )
		{
			movementTarget = g_game->m_player->m_position;
		}
		else
		{
			UpdateDijkstraMap();
			UpdateWaypointChase();
			if ( m_waypointPosition != Vec2::ZERO )
			{
				movementTarget = m_waypointPosition;
			}
		}
	}
	else if ( m_lastSeenPlayerPosition != Vec2::ZERO )
	{
		UpdateDijkstraMap();
		m_goalPosition = m_lastSeenPlayerPosition;
		UpdateWaypointChase();
		if ( m_waypointPosition != Vec2::ZERO )
		{
			movementTarget = m_waypointPosition;
		}
	}

	if ( movementTarget == Vec2::ZERO && m_lastSeenPlayerPosition == Vec2::ZERO )
	{
		if ( m_wanderGoalPosition == Vec2::ZERO )
		{
			PickNewWanderGoal();
		}

		if ( m_wanderGoalPosition != Vec2::ZERO )
		{
			UpdateDijkstraMap();
			UpdateWaypointWander();

			if ( m_waypointPosition != Vec2::ZERO )
			{
				movementTarget = m_waypointPosition;
			}
			else
			{
				movementTarget = m_wanderGoalPosition;
			}
		}
	}

	if ( movementTarget == Vec2::ZERO )
	{
		m_timeSinceLastTurn += deltaSeconds;
		bool needNewTarget = false;

		if ( m_movementTargetPosition == Vec2::ZERO || ( m_movementTargetPosition - m_position ).GetLength() <= m_physicsRadius )
		{
			needNewTarget = true;
		}

		if ( m_timeSinceLastTurn >= g_gameConfigBlackboard.GetValue( "ariesTurnCooldown", 1.5f ) || needNewTarget )
		{
			for ( int attempts = 0; attempts < 10; ++attempts )
			{
				float ang = m_rng.RollRandomFloatInRange( 0.f, 360.f );
				Vec2  cand = m_position + Vec2::MakeFromPolarDegrees( ang, 20.f );

				IntVec2 candTile = g_game->m_currentMap->GetTileCoordsForWorldPosition( cand );
				Tile* candTilePtr = g_game->m_currentMap->GetTile( candTile );

				if ( candTilePtr != nullptr && !g_game->m_currentMap->IsTileSolid( *candTilePtr, !m_canSwim ) )
				{
					m_movementTargetPosition = cand;
					break;
				}
			}
			m_timeSinceLastTurn = 0.f;
		}

		movementTarget = m_movementTargetPosition;
	}
	else
	{
		m_movementTargetPosition = movementTarget;
	}

	if ( m_movementTargetPosition != Vec2::ZERO )
	{
		MoveTowardTargetPosition( deltaSeconds );
	}

	if ( ( m_movementTargetPosition - m_position ).GetLength() <= m_physicsRadius )
	{
		m_movementTargetPosition = Vec2::ZERO;
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
	// Turn toward movement target
	float targetOrientationDegrees = ( m_movementTargetPosition - m_position ).GetOrientationDegrees();
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


//-----------------------------------------------------------------------------------------------
void Aries::UpdateDijkstraMap()
{
	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return;
	}

	IntVec2 originTile( -1, -1 );

	if ( m_lastSeenPlayerPosition != Vec2::ZERO )
	{
		originTile = map->GetTileCoordsForWorldPosition( m_lastSeenPlayerPosition );
	}
	else if ( m_wanderGoalPosition != Vec2::ZERO && m_wanderGoalTile.x >= 0 && m_wanderGoalTile.y >= 0 )
	{
		originTile = m_wanderGoalTile;
	}
	else
	{
		return;
	}

	if ( m_dijkstraMap == nullptr || !( originTile == m_dijkstraOriginTile ) )
	{
		if ( m_dijkstraMap != nullptr )
		{
			delete m_dijkstraMap;
			m_dijkstraMap = nullptr;
		}

		IntVec2 dims = map->m_dimensions;
		m_dijkstraMap = new TileHeatMap( dims );
		map->PopulateDijkstraMap( *m_dijkstraMap, originTile, 999999.f, !m_canSwim );
		m_dijkstraOriginTile = originTile;

		m_waypointPosition = Vec2::ZERO;
	}
}


//-----------------------------------------------------------------------------------------------
void Aries::UpdateWaypointChase()
{
	if ( m_dijkstraMap == nullptr || m_lastSeenPlayerPosition == Vec2::ZERO )
	{
		return;
	}

	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return;
	}

	IntVec2 fromTile = map->GetTileCoordsForWorldPosition( m_position );

	if ( fromTile == m_dijkstraOriginTile )
	{
		m_lastSeenPlayerPosition = Vec2::ZERO;
		m_waypointPosition = Vec2::ZERO;
		m_goalPosition = Vec2::ZERO;
		return;
	}

	float   bestCost = m_dijkstraMap->GetValueAtTileCoords( fromTile );
	IntVec2 bestTile = fromTile;

	for ( int dy = -1; dy <= 1; ++dy )
	{
		for ( int dx = -1; dx <= 1; ++dx )
		{
			if ( dx == 0 && dy == 0 )
			{
				continue;
			}

			IntVec2 n( fromTile.x + dx, fromTile.y + dy );

			if ( n.x < 0 || n.y < 0 ||
				n.x >= m_dijkstraMap->m_dimensions.x ||
				n.y >= m_dijkstraMap->m_dimensions.y )
			{
				continue;
			}

			float cost = m_dijkstraMap->GetValueAtTileCoords( n );
			if ( cost < bestCost && cost < 999999.f )
			{
				bestCost = cost;
				bestTile = n;
			}
		}
	}

	if ( !( bestTile == fromTile ) )
	{
		m_waypointPosition = map->GetWorldPositionForTileCoords( bestTile );
	}
	else
	{
		m_lastSeenPlayerPosition = Vec2::ZERO;
		m_waypointPosition = Vec2::ZERO;
		m_goalPosition = Vec2::ZERO;
	}
}


//-----------------------------------------------------------------------------------------------
void Aries::UpdateWaypointWander()
{
	if ( m_dijkstraMap == nullptr || m_wanderGoalPosition == Vec2::ZERO )
	{
		return;
	}

	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return;
	}

	IntVec2 fromTile = map->GetTileCoordsForWorldPosition( m_position );

	if ( fromTile == m_wanderGoalTile )
	{
		PickNewWanderGoal();
		m_goalPosition = m_wanderGoalPosition;
		return;
	}

	float   bestCost = m_dijkstraMap->GetValueAtTileCoords( fromTile );
	IntVec2 bestTile = fromTile;

	for ( int dy = -1; dy <= 1; ++dy )
	{
		for ( int dx = -1; dx <= 1; ++dx )
		{
			if ( dx == 0 && dy == 0 )
			{
				continue;
			}

			IntVec2 n( fromTile.x + dx, fromTile.y + dy );

			if ( n.x < 0 || n.y < 0 ||
				n.x >= m_dijkstraMap->m_dimensions.x ||
				n.y >= m_dijkstraMap->m_dimensions.y )
			{
				continue;
			}

			float cost = m_dijkstraMap->GetValueAtTileCoords( n );
			if ( cost < bestCost && cost < 999999.f )
			{
				bestCost = cost;
				bestTile = n;
			}
		}
	}

	if ( !( bestTile == fromTile ) )
	{
		m_waypointPosition = map->GetWorldPositionForTileCoords( bestTile );
	}
	else
	{
		PickNewWanderGoal();
	}
}


//-----------------------------------------------------------------------------------------------
bool Aries::IsPlayerInAdjacentTile() const
{
	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return false;
	}

	IntVec2 ariesTile = map->GetTileCoordsForWorldPosition( m_position );
	IntVec2 playerTile = map->GetTileCoordsForWorldPosition( g_game->m_player->m_position );

	int dx = abs( playerTile.x - ariesTile.x );
	int dy = abs( playerTile.y - ariesTile.y );
	return ( dx <= 1 && dy <= 1 );
}


//-----------------------------------------------------------------------------------------------
void Aries::PickNewWanderGoal()
{
	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return;
	}

	IntVec2 dims = map->m_dimensions;

	const int maxAttempts = 100;

	for ( int attempt = 0; attempt < maxAttempts; ++attempt )
	{
		IntVec2 goalTile;
		goalTile.x = m_rng.RollRandomIntInRange( 1, dims.x - 2 );
		goalTile.y = m_rng.RollRandomIntInRange( 1, dims.y - 2 );

		if ( !IsTileTraversableForWander( goalTile ) )
		{
			continue;
		}

		if ( m_dijkstraMap == nullptr )
		{
			m_dijkstraMap = new TileHeatMap( dims );
		}

		map->PopulateDijkstraMap( *m_dijkstraMap, goalTile, 999999.f, !m_canSwim );

		IntVec2 ariesTile = map->GetTileCoordsForWorldPosition( m_position );
		float   ariesCost = m_dijkstraMap->GetValueAtTileCoords( ariesTile );
		if ( ariesCost >= 999999.f )
		{
			// Unreachable
			continue;
		}

		// Accept this wander goal
		m_wanderGoalTile = goalTile;
		m_wanderGoalPosition = map->GetWorldPositionForTileCoords( goalTile );
		m_goalPosition = m_wanderGoalPosition;
		m_dijkstraOriginTile = goalTile;
		m_waypointPosition = Vec2::ZERO;
		return;
	}

	// Could not find any good wander goal
	m_wanderGoalTile = IntVec2( -1, -1 );
	m_wanderGoalPosition = Vec2::ZERO;
}


//-----------------------------------------------------------------------------------------------
bool Aries::IsTileOccupiedByScorpio( const IntVec2& tileCoords ) const
{
	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return false;
	}

	int scorpioCount = static_cast< int >( map->m_entityListsByType[ENTITY_TYPE_EVIL_SCORPIO].size() );
	for ( int i = 0; i < scorpioCount; ++i )
	{
		Entity* scorpio = map->m_entityListsByType[ENTITY_TYPE_EVIL_SCORPIO][i];
		if ( scorpio == nullptr )
		{
			continue;
		}

		IntVec2 scorpioCoords = map->GetTileCoordsForWorldPosition( scorpio->m_position );
		if ( scorpioCoords == tileCoords )
		{
			return true;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool Aries::IsTileTraversableForWander( const IntVec2& tileCoords ) const
{
	Map* map = g_game->m_currentMap;
	if ( map == nullptr )
	{
		return false;
	}

	Tile* tile = map->GetTile( tileCoords );
	if ( tile == nullptr )
	{
		return false;
	}

	if ( map->IsTileSolid( *tile, !m_canSwim ) )
	{
		return false;
	}

	if ( IsTileOccupiedByScorpio( tileCoords ) )
	{
		return false;
	}

	return true;
}
