#include "Game/Map.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/Scorpio.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( IntVec2 dimensions )
	: m_dimensions( dimensions )
{
	PopulateTiles();

	// Spawn Player
	IntVec2 playerStartTileCoords = IntVec2( 1, 1 );
	Vec2 playerStartPos = GetWorldPositionForTileCoords( playerStartTileCoords );
	Player* player = static_cast<Player*>( SpawnNewEntity( ENTITY_TYPE_GOOD_PLAYER, playerStartPos, 45.f ) );
	g_game->m_player = player;
	AddEntityToMap( *g_game->m_player, ENTITY_TYPE_GOOD_PLAYER );

	// Spawn Scorpio
	IntVec2 scorpioStartTileCoords = IntVec2( 4, 1 );
	Vec2 scorpioStartPos = GetWorldPositionForTileCoords( scorpioStartTileCoords );
	Scorpio* scorpio = static_cast<Scorpio*>( SpawnNewEntity( ENTITY_TYPE_EVIL_SCORPIO, scorpioStartPos, 0.f ) );
	AddEntityToMap( *scorpio, ENTITY_TYPE_EVIL_SCORPIO );
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	// Do nothing
}


//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds )
{
	UpdateEntities( deltaSeconds );
	DeleteGarbageEntities();
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateEntities( float deltaSeconds )
{
	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++ entityIndex )
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity != nullptr )
		{
			entity->Update( deltaSeconds );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	UpdateWorldCameraView();

	RenderTiles();
	RenderEntities();
}


//-----------------------------------------------------------------------------------------------
void Map::RenderTiles() const
{
	int numTiles = static_cast< int >( m_tiles.size() );
	std::vector<Vertex> tileVerts;
	for ( int tileIndex = 0; tileIndex < numTiles; ++ tileIndex )
	{
		const Tile& tile = m_tiles[tileIndex];
		AABB2 tileBounds = AABB2( tile.m_tileCoords.x * TILE_SIZE, tile.m_tileCoords.y * TILE_SIZE,
			( tile.m_tileCoords.x + 1 ) * TILE_SIZE, ( tile.m_tileCoords.y + 1 ) * TILE_SIZE );
		Rgba8 tileColor;
		if ( tile.m_type == TileType::TILE_TYPE_GRASS )
		{
			tileColor = GRASS_COLOR;
		}
		else if ( tile.m_type == TileType::TILE_TYPE_STONE )
		{
			tileColor = STONE_COLOR;
		}
		AddVertsForAABB2D( tileVerts, tileBounds, tileColor );
	}
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( static_cast< int >( tileVerts.size() ), tileVerts.data() );
}


//-----------------------------------------------------------------------------------------------
void Map::RenderEntities() const
{
	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++ entityIndex )
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity != nullptr )
		{
			entity->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DebugRender() const
{
	if (!g_game->m_isDebugOn) {
		return;
	}

	const float ringThickness = 0.3f;
	const float thinLineThickness = 0.3f;
	const float thickLineThickness = 0.5f;

	Rgba8 cyan = Rgba8( 0, 255, 255 );
	Rgba8 magenta = Rgba8( 255, 0, 255 );
	Rgba8 yellow = Rgba8( 255, 255, 0 );
	Rgba8 red = Rgba8( 255, 0, 0 );
	Rgba8 green = Rgba8( 0, 255, 0 );
	Rgba8 blue = Rgba8( 0, 0, 255 );
	Rgba8 purple = Rgba8( 128, 0, 255 );

	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++entityIndex ) 
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity == nullptr ) continue;

		Player* player = dynamic_cast<Player*>( entity );
		if ( player != nullptr ) 
		{
			// Draw turret current orientation
			float turretDegrees = player->m_turretOrientationDegrees;
			Vec2 turretLineEnd = player->m_position + Vec2::MakeFromPolarDegrees( turretDegrees, player->m_cosmeticRadius * 0.6f );
			DebugDrawLine( player->m_position, turretLineEnd, thickLineThickness, purple, purple );

			// Draw turret goal orientation
			float turretGoalDegrees = player->m_turretTargetOrientationDegrees;
			Vec2 turretGoalStart = player->m_position + Vec2::MakeFromPolarDegrees( turretGoalDegrees, player->m_cosmeticRadius * 1.05f );
			Vec2 turretGoalEnd = player->m_position + Vec2::MakeFromPolarDegrees( turretGoalDegrees, player->m_cosmeticRadius * 1.18f );
			DebugDrawLine( turretGoalStart, turretGoalEnd, thickLineThickness, purple, purple );

			// Draw tank current orientation
			float tankDegrees = player->m_orientationDegrees;
			Vec2 tankLineEnd = player->m_position + Vec2::MakeFromPolarDegrees( tankDegrees, player->m_cosmeticRadius * 0.8f );
			DebugDrawLine( player->m_position, tankLineEnd, thinLineThickness, blue, blue );

			// Draw tank goal orientation
			float goalDegrees = player->m_targetMovementDirection;
			Vec2 goalShortStart = player->m_position + Vec2::MakeFromPolarDegrees( goalDegrees, player->m_cosmeticRadius * 1.05f );
			Vec2 goalShortEnd = player->m_position + Vec2::MakeFromPolarDegrees( goalDegrees, player->m_cosmeticRadius * 1.4f );
			DebugDrawLine( goalShortStart, goalShortEnd, thinLineThickness, blue, blue );
		}

		if (entity != nullptr) 
		{
			// Draw physics and cosmetic radius rings
			DebugDrawRing( entity->m_position, entity->m_physicsRadius, ringThickness, cyan );
			DebugDrawRing( entity->m_position, entity->m_cosmeticRadius, ringThickness, magenta );

			// Draw velocity vector
			Vec2 velocityEnd = entity->m_position + entity->m_velocity;
			DebugDrawLine( entity->m_position, velocityEnd, thinLineThickness, yellow, yellow );

			// Draw forward and left vectors
			Vec2 forward = Vec2::MakeFromPolarDegrees( entity->m_orientationDegrees, entity->m_cosmeticRadius * 0.8f );
			Vec2 left = forward.GetRotatedBy90Degrees();
			DebugDrawLine( entity->m_position, entity->m_position + forward, thinLineThickness, red, red );
			DebugDrawLine( entity->m_position, entity->m_position + left, thinLineThickness, green, green );

			// Draw line of sight only hit a solid tile
			RaycastResult2D raycastResult = RaycastVsTiles( entity->m_position, forward.GetNormalized(), 500.f, 0.1f );
			if ( raycastResult.m_didImpact ) 
			{
				DebugDrawLine( entity->m_position, raycastResult.m_impactPos, thinLineThickness, red, red );
			} 
			else 
			{
				DebugDrawLine( entity->m_position, entity->m_position + forward, thinLineThickness, green, green );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Tile* Map::GetTile( IntVec2 tileCoords ) const
{
	if ( !IsTileCoordsInBounds( tileCoords ) )
	{
		return nullptr;
	}
	int tileIndex = tileCoords.y * m_dimensions.x + tileCoords.x;
	return const_cast<Tile*>( &m_tiles[tileIndex] );
}


//-----------------------------------------------------------------------------------------------
AABB2 Map::GetTileBounds( IntVec2 tileCoords ) const
{
	float minX = static_cast<float>( tileCoords.x ) * TILE_SIZE;
	float minY = static_cast<float>( tileCoords.y ) * TILE_SIZE;
	float maxX = static_cast<float>( tileCoords.x + 1 ) * TILE_SIZE;
	float maxY = static_cast<float>( tileCoords.y + 1 ) * TILE_SIZE;
	return AABB2( minX, minY, maxX, maxY );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileCoordsInBounds( IntVec2 tileCoords ) const
{
	if ( tileCoords.x < 0 || tileCoords.y < 0 ||
		 tileCoords.x >= m_dimensions.x || tileCoords.y >= m_dimensions.y )
	{
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------
IntVec2 Map::GetTileCoordsForWorldPosition( Vec2 worldPos ) const
{
	int tileX = static_cast<int>( worldPos.x / TILE_SIZE );
	int tileY = static_cast<int>( worldPos.y / TILE_SIZE );
	return IntVec2( tileX, tileY );
}


//-----------------------------------------------------------------------------------------------
Vec2 Map::GetWorldPositionForTileCoords( IntVec2 tileCoords ) const
{
	float worldX = ( static_cast<float>( tileCoords.x ) + 0.5f ) * TILE_SIZE;
	float worldY = ( static_cast<float>( tileCoords.y ) + 0.5f ) * TILE_SIZE;
	return Vec2( worldX, worldY );
}


//-----------------------------------------------------------------------------------------------
void Map::PopulateTiles()
{
	if ( TileDefinition::s_definitions.size() != NUM_TILE_TYPES )
	{
		TileDefinition::s_definitions.resize( NUM_TILE_TYPES );
		TileDefinition::InitializeTileDefinitions();
	}

	m_tiles.clear();

	for ( int y = 0; y < m_dimensions.y; ++y )
	{
		for ( int x = 0; x < m_dimensions.x; ++x )
		{
			IntVec2 tileCoords = IntVec2( x, y );
			TileType type;

			bool isOnEdge = ( x == 0 || y == 0 || x == m_dimensions.x - 1 || y == m_dimensions.y - 1 );
			bool isInnerReserved = ( x >= 1 && x <= 5 && y >= 1 && y <= 5 );
			bool isOuterReserved = ( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 && y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 );
			if ( isOnEdge )
			{
				type = TileType::TILE_TYPE_STONE;
			}
			else
			{
				if ( isInnerReserved )
				{
					if ( ( x == 2 && y == 4 ) || ( x == 3 && y == 4 ) || ( x == 4 && y == 4 ) ||
						( x == 4 && y == 3 ) || ( x == 4 && y == 2 ) )
					{
						type = TileType::TILE_TYPE_STONE;
					}
					else
					{
						type = TileType::TILE_TYPE_GRASS;
					}
				}
				else if ( isOuterReserved )
				{
					if ( ( x == m_dimensions.x - 6 && y == m_dimensions.y - 3 ) ||
						( x == m_dimensions.x - 6 && y == m_dimensions.y - 4 ) ||
						( x == m_dimensions.x - 6 && y == m_dimensions.y - 5 ) ||
						( x == m_dimensions.x - 6 && y == m_dimensions.y - 6 ) ||
						( x == m_dimensions.x - 5 && y == m_dimensions.y - 6 ) ||
						( x == m_dimensions.x - 4 && y == m_dimensions.y - 6 ) ||
						( x == m_dimensions.x - 3 && y == m_dimensions.y - 6 ) )
					{
						type = TileType::TILE_TYPE_STONE;
					}
					else
					{
						type = TileType::TILE_TYPE_GRASS;
					}
				}
				else
				{
					RandomNumberGenerator rng;
					float roll = rng.RollRandomFloatZeroToOne();
					if ( roll < INNER_STONE_TILE_PROBABILITY )
					{
						type = TileType::TILE_TYPE_STONE;
					}
					else
					{
						type = TileType::TILE_TYPE_GRASS;
					}
				}
			}
			Tile newTile = Tile( tileCoords, type );
			m_tiles.push_back( newTile );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DeleteGarbageEntities()
{
	for ( int entityIndex = static_cast<int>( m_allEntities.size() ) - 1; entityIndex >= 0; -- entityIndex )
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity != nullptr && entity->m_isGarbage )
		{
			delete entity;
			entity = nullptr;
			m_allEntities.erase( m_allEntities.begin() + entityIndex );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateWorldCameraView() const
{
	float tilesInViewY = static_cast< float >( g_game->m_numTilesInViewVertically );
	float aspect = g_engine->m_window->m_config.m_clientAspect;
	float viewHeight = tilesInViewY * TILE_SIZE;
	float viewWidth = viewHeight * aspect;

	float mapWidth = static_cast< float >( m_dimensions.x ) * TILE_SIZE;
	float mapHeight = static_cast< float >( m_dimensions.y ) * TILE_SIZE;

	Vec2 cameraCenter = Vec2( mapWidth * 0.5f, mapHeight * 0.5f );
	if ( g_game->m_player && g_game->m_player->IsAlive() ) {
		cameraCenter = g_game->m_player->m_position;
	}

	float halfViewWidth = viewWidth * 0.5f;
	float halfViewHeight = viewHeight * 0.5f;

	float minX = halfViewWidth;
	float maxX = mapWidth - halfViewWidth;
	float minY = halfViewHeight;
	float maxY = mapHeight - halfViewHeight;

	if ( mapWidth < viewWidth ) {
		cameraCenter.x = mapWidth * 0.5f;
	}
	else {
		cameraCenter.x = GetClamped( cameraCenter.x, minX, maxX );
	}
	if ( mapHeight < viewHeight ) {
		cameraCenter.y = mapHeight * 0.5f;
	}
	else {
		cameraCenter.y = GetClamped( cameraCenter.y, minY, maxY );
	}

	Vec2 cameraBottomLeft = Vec2( cameraCenter.x - halfViewWidth, cameraCenter.y - halfViewHeight );
	Vec2 cameraTopRight = Vec2( cameraCenter.x + halfViewWidth, cameraCenter.y + halfViewHeight );
	g_game->m_worldCamera->SetOrthoView( cameraBottomLeft, cameraTopRight );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsPointInSolidTile( Vec2 const& point ) const
{
	IntVec2 tileCoords = GetTileCoordsForWorldPosition( point );
	if ( !IsTileCoordsInBounds( tileCoords ) )
	{
		return true;
	}
	Tile* tile = GetTile( tileCoords );
	return IsTileSolid( *tile );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileSolid( Tile tile ) const
{
	return tile.IsSolid();
}


//-----------------------------------------------------------------------------------------------
RaycastResult2D Map::RaycastVsTiles( Vec2 const& startPos, Vec2 const& fwdNormal, float maxDist, float stepSize ) const
{
	RaycastResult2D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;

	Vec2 currentPos = startPos;
	float traveledDist = 0.f;
	while ( traveledDist < maxDist )
	{
		IntVec2 tileCoords = GetTileCoordsForWorldPosition( currentPos );
		Tile* tile = GetTile( tileCoords );
		if ( !IsTileCoordsInBounds( tileCoords ) )
		{
			break;
		}
		if ( tile != nullptr && IsTileSolid( *tile ) )
		{
			result.m_didImpact = true;
			result.m_impactDist = traveledDist;
			result.m_impactPos = currentPos;

			AABB2 tileBounds = GetTileBounds( tileCoords );
			Vec2 nearestPoint = tileBounds.GetNearestPoint( currentPos );
			Vec2 impactNormal = ( currentPos - nearestPoint ).GetNormalized();
			result.m_impactNormal = impactNormal;

			return result;
		}
		currentPos += fwdNormal * stepSize;
		traveledDist += stepSize;
	}

	return result;
}


//-----------------------------------------------------------------------------------------------
bool Map::HasLineOfSight( Vec2 const& startPos, Vec2 const& endPos, float stepSize ) const
{
	Vec2 displacement = endPos - startPos;
	float distance = displacement.GetLength();
	Vec2 direction = displacement.GetNormalized();
	RaycastResult2D raycast = RaycastVsTiles( startPos, direction, distance, stepSize );
	return !raycast.m_didImpact;
}


//-----------------------------------------------------------------------------------------------
Entity* Map::SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees )
{
	Entity* newEntity = nullptr;
	if ( type == ENTITY_TYPE_GOOD_PLAYER )
	{
		newEntity = new Player( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_EVIL_SCORPIO )
	{
		newEntity = new Scorpio( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_EVIL_LEO )
	{
		// newEntity = new Leo( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_EVIL_ARIES )
	{
		// newEntity = new Aries( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_GOOD_BULLET )
	{
		// newEntity = new GoodBullet( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_GOOD_BOLT )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_GOOD_BOLT );
	}
	else if ( type == ENTITY_TYPE_EVIL_BULLET )
	{
		// newEntity = new EvilBullet( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_EVIL_BOLT )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_EVIL_BOLT );
	}
	else
	{
		return nullptr;
	}
	return newEntity;
}


//-----------------------------------------------------------------------------------------------
void Map::AddEntityToMap( Entity& entity, EntityType type )
{
	m_allEntities.push_back( &entity );
	m_entityListsByType[type].push_back( &entity );
}


//-----------------------------------------------------------------------------------------------
void Map::RemoveEntityFromMap( Entity& entity, EntityType type )
{
	// Remove from all entities list
	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++ entityIndex )
	{
		if ( m_allEntities[entityIndex] == &entity )
		{
			m_allEntities.erase( m_allEntities.begin() + entityIndex );
			break;
		}
	}

	// Remove from type-specific entity list
	EntityList& typeList = m_entityListsByType[type];
	for ( int entityIndex = 0; entityIndex < static_cast<int>( typeList.size() ); ++ entityIndex )
	{
		if ( typeList[entityIndex] == &entity )
		{
			typeList.erase( typeList.begin() + entityIndex );
			break;
		}
	}
}