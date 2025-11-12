#include "Game/Map.hpp"
#include "Game/Aries.hpp"
#include "Game/Bullet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Leo.hpp"
#include "Game/MapDefinition.hpp"
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
#include "Engine/Renderer/SpriteDefinition.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( IntVec2 dimensions, int index )
	: m_dimensions( dimensions )
	, m_index( index )
{
	PopulateTiles();
	SpawnEntitiesForMapDefinition();

	// Add player to the map
	AddEntityToMap( *g_game->m_player, ENTITY_TYPE_GOOD_PLAYER, ENTITY_FACTION_GOOD );
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

	ResolveEntityVsEntityCollision();
	ResolveEntityVsTileCollision();
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
	std::vector<Vertex> tileVerts;

	g_engine->m_renderer->BindTexture( &g_game->m_tileSpriteSheet->GetTexture() );

	for ( Tile const& tile : m_tiles )
	{
		AABB2 tileBounds( tile.m_tileCoords.x * TILE_SIZE, tile.m_tileCoords.y * TILE_SIZE,
						( tile.m_tileCoords.x + 1 ) * TILE_SIZE, ( tile.m_tileCoords.y + 1 ) * TILE_SIZE );

		TileDefinition const& tileDef = tile.GetDefinition();
		Rgba8 tint = tileDef.m_tint;
		Vec2 uvMins = tileDef.m_UVs.m_mins;
		Vec2 uvMaxs = tileDef.m_UVs.m_maxs;

		AddVertsForAABB2D( tileVerts, tileBounds, tint, uvMins, uvMaxs );
	}

	g_engine->m_renderer->DrawVertexArray( static_cast<int>( tileVerts.size() ), tileVerts.data() );
	g_engine->m_renderer->BindTexture( nullptr );
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
				type = MapDefinition::s_definitions[m_index].m_borderTileType;
			}
			else
			{
				if ( isInnerReserved )
				{
					if ( x == 1 && y == 1 )
					{
						type = TILE_TYPE_ENTRANCE;
					}
					else if ( ( x == 2 && y == 4 ) || ( x == 3 && y == 4 ) || ( x == 4 && y == 4 ) ||
							  ( x == 4 && y == 3 ) || ( x == 4 && y == 2 ) )
					{
						type = MapDefinition::s_definitions[m_index].m_startbunkerWallTileType;
					}
					else
					{
						type = MapDefinition::s_definitions[m_index].m_bunkerFloorTileType;
					}
				}
				else if ( isOuterReserved )
				{
					if ( x == m_dimensions.x - 2 && y == m_dimensions.y - 2 )
					{
						type = TILE_TYPE_EXIT;
					}
					else if ( ( x == m_dimensions.x - 6 && y == m_dimensions.y - 3 ) ||
							  ( x == m_dimensions.x - 6 && y == m_dimensions.y - 4 ) ||
							  ( x == m_dimensions.x - 6 && y == m_dimensions.y - 5 ) ||
							  ( x == m_dimensions.x - 6 && y == m_dimensions.y - 6 ) ||
							  ( x == m_dimensions.x - 5 && y == m_dimensions.y - 6 ) ||
							  ( x == m_dimensions.x - 4 && y == m_dimensions.y - 6 ) ||
							  ( x == m_dimensions.x - 3 && y == m_dimensions.y - 6 ) )
					{
						type = MapDefinition::s_definitions[m_index].m_exitbunkerWallTileType;
					}
					else
					{
						type = MapDefinition::s_definitions[m_index].m_bunkerFloorTileType;
					}
				}
				else
				{
					RandomNumberGenerator rng;

					float roll = rng.RollRandomFloatZeroToOne();
					if ( roll < MapDefinition::s_definitions[m_index].m_sprinkle1Probability )
					{
						type = MapDefinition::s_definitions[m_index].m_sprinkle1TileType;
					}
					else
					{
						roll = rng.RollRandomFloatZeroToOne();
						if ( roll < MapDefinition::s_definitions[m_index].m_sprinkle2Probability )
						{
							type = MapDefinition::s_definitions[m_index].m_sprinkle2TileType;
						}
						else
						{
							type = MapDefinition::s_definitions[m_index].m_fillTileType;
						}
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
	if ( g_game->m_player ) {
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
void Map::SpawnEntitiesForMapDefinition()
{
	int mapIndex = m_index;
	MapDefinition const& mapDef = MapDefinition::s_definitions[mapIndex];

	RandomNumberGenerator rng;

	// Spawn Scorpios
	for ( int i = 0; i < mapDef.m_numOfScorpios; ++ i )
	{
		Vec2 rawSpawnPos;

		// Ensure not spawning inside the entrance or exit bunkers
		bool isRespawnPosValid = false;
		while ( !isRespawnPosValid )
		{
			rawSpawnPos.x = rng.RollRandomFloatInRange( 2.f * TILE_SIZE, ( static_cast<float>( m_dimensions.x ) - 3.f ) * TILE_SIZE );
			rawSpawnPos.y = rng.RollRandomFloatInRange( 2.f * TILE_SIZE, ( static_cast<float>( m_dimensions.y ) - 3.f ) * TILE_SIZE );
			IntVec2 spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
			isRespawnPosValid = true;

			// Check inner bunker area
			if ( spawnTileCoords.x >= 1 && spawnTileCoords.x <= 6 &&
				 spawnTileCoords.y >= 1 && spawnTileCoords.y <= 6 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Check outer bunker area
			if ( spawnTileCoords.x >= m_dimensions.x - 7 && spawnTileCoords.x <= m_dimensions.x - 2 &&
				 spawnTileCoords.y >= m_dimensions.y - 7 && spawnTileCoords.y <= m_dimensions.y - 2 )
			{
				isRespawnPosValid = false;
				continue;
			}
		}

		IntVec2 spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
		Vec2 spawnPos = GetWorldPositionForTileCoords( spawnTileCoords );

		// Ensure not spawning on solid tile
		if ( IsPointInSolidTile( spawnPos ) )
		{
			AABB2 tileBounds = GetTileBounds( spawnTileCoords );
			PushDiscOutOfFixedAABB2D( spawnPos, TILE_SIZE, tileBounds );
		}

		Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EVIL_SCORPIO, spawnPos, 0.f );
		if ( newEntity != nullptr )
		{
			AddEntityToMap( *newEntity, ENTITY_TYPE_EVIL_SCORPIO, ENTITY_FACTION_EVIL );
		}
	}

	// Spawn Leos
	for ( int i = 0; i < mapDef.m_numOfLeos; ++ i )
	{
		Vec2 rawSpawnPos;

		// Ensure not spawning inside the entrance or exit bunkers
		bool isRespawnPosValid = false;
		while ( !isRespawnPosValid )
		{
			rawSpawnPos.x = rng.RollRandomFloatInRange( 2.f * TILE_SIZE, ( static_cast<float>( m_dimensions.x ) - 3.f ) * TILE_SIZE );
			rawSpawnPos.y = rng.RollRandomFloatInRange( 2.f * TILE_SIZE, ( static_cast<float>( m_dimensions.y ) - 3.f ) * TILE_SIZE );
			IntVec2 spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
			isRespawnPosValid = true;
			// Check inner bunker area
			if ( spawnTileCoords.x >= 1 && spawnTileCoords.x <= 6 &&
				 spawnTileCoords.y >= 1 && spawnTileCoords.y <= 6 )
			{
				isRespawnPosValid = false;
				continue;
			}
			// Check outer bunker area
			if ( spawnTileCoords.x >= m_dimensions.x - 7 && spawnTileCoords.x <= m_dimensions.x - 2 &&
				 spawnTileCoords.y >= m_dimensions.y - 7 && spawnTileCoords.y <= m_dimensions.y - 2 )
			{
				isRespawnPosValid = false;
				continue;
			}
		}

		IntVec2 spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
		Vec2 spawnPos = GetWorldPositionForTileCoords( spawnTileCoords );
		
		Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EVIL_LEO, spawnPos, 0.f );
		if ( newEntity != nullptr )
		{
			AddEntityToMap( *newEntity, ENTITY_TYPE_EVIL_LEO, ENTITY_FACTION_EVIL );
		}
	}

	// Spawn Aries
	for ( int i = 0; i < mapDef.m_numOfAries; ++ i )
	{
		Vec2 rawSpawnPos;
		
		// Ensure not spawning inside the entrance or exit bunkers
		bool isRespawnPosValid = false;
		while ( !isRespawnPosValid )
		{
			rawSpawnPos.x = rng.RollRandomFloatInRange( 2.f * TILE_SIZE, ( static_cast<float>( m_dimensions.x ) - 3.f ) * TILE_SIZE );
			rawSpawnPos.y = rng.RollRandomFloatInRange( 2.f * TILE_SIZE, ( static_cast<float>( m_dimensions.y ) - 3.f ) * TILE_SIZE );
			IntVec2 spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
			isRespawnPosValid = true;
			// Check inner bunker area
			if ( spawnTileCoords.x >= 1 && spawnTileCoords.x <= 6 &&
				 spawnTileCoords.y >= 1 && spawnTileCoords.y <= 6 )
			{
				isRespawnPosValid = false;
				continue;
			}
			// Check outer bunker area
			if ( spawnTileCoords.x >= m_dimensions.x - 7 && spawnTileCoords.x <= m_dimensions.x - 2 &&
				 spawnTileCoords.y >= m_dimensions.y - 7 && spawnTileCoords.y <= m_dimensions.y - 2 )
			{
				isRespawnPosValid = false;
				continue;
			}
		}

		IntVec2 spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
		Vec2 spawnPos = GetWorldPositionForTileCoords( spawnTileCoords );
		
		Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EVIL_ARIES, spawnPos, 0.f );
		if ( newEntity != nullptr )
		{
			AddEntityToMap( *newEntity, ENTITY_TYPE_EVIL_ARIES, ENTITY_FACTION_EVIL );
		}
	}
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
		newEntity = new Leo( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_EVIL_ARIES )
	{
		newEntity = new Aries( position, orientationDegrees );
	}
	else if ( type == ENTITY_TYPE_GOOD_BULLET )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_GOOD_BULLET );
	}
	else if ( type == ENTITY_TYPE_GOOD_BOLT )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_GOOD_BOLT );
	}
	else if ( type == ENTITY_TYPE_EVIL_BULLET )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_EVIL_BULLET );
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
void Map::AddEntityToMap( Entity& entity, EntityType type, EntityFaction faction )
{
	// Add to all entities list
	bool addedToAllEntities = false;
	for ( int i = 0; i < static_cast<int>( m_allEntities.size() ); ++ i )
	{
		Entity* existingEntity = m_allEntities[i];
		if ( existingEntity == &entity ) break;
		if ( existingEntity == nullptr ) 
		{
			addedToAllEntities = true;
			m_allEntities[i] = &entity;
			break;
		}
	}
	if ( !addedToAllEntities ) m_allEntities.push_back( &entity );

	// Add to type-specific entity list
	bool addedToTypeList = false;
	EntityList& typeList = m_entityListsByType[type];
	for ( int i = 0; i < static_cast<int>( typeList.size() ); ++ i )
	{
		Entity* existingEntity = typeList[i];
		if ( existingEntity == &entity ) break;
		if ( existingEntity == nullptr ) 
		{
			addedToTypeList = true;
			typeList[i] = &entity;
			break;
		}
	}
	if ( !addedToTypeList ) m_entityListsByType[type].push_back( &entity );

	// Add to faction-specific entity list
	bool addedToFactionList = false;
	EntityList& factionList = m_entityListsByFaction[faction];
	for ( int i = 0; i < static_cast<int>( factionList.size() ); ++ i )
	{
		Entity* existingEntity = factionList[i];
		if ( existingEntity == &entity ) break;
		if ( existingEntity == nullptr ) 
		{
			addedToFactionList = true;
			factionList[i] = &entity;
			break;
		}
	}
	if ( !addedToFactionList ) m_entityListsByFaction[faction].push_back( &entity );
}


//-----------------------------------------------------------------------------------------------
void Map::RemoveEntityFromMap( Entity& entity, EntityType type, EntityFaction faction )
{
	// Remove from all entities list
	for ( int i = 0; i < static_cast<int>( m_allEntities.size() ); ++ i )
	{
		Entity* existingEntity = m_allEntities[i];
		if ( existingEntity == &entity )
		{
			existingEntity = nullptr;
			break;
		}
	}

	// Remove from type-specific entity list
	EntityList& typeList = m_entityListsByType[type];
	for ( int i = 0; i < static_cast<int>( typeList.size() ); ++ i )
	{
		Entity* existingEntity = typeList[i];
		if ( existingEntity == &entity )
		{
			existingEntity = nullptr;
			break;
		}
	}

	// Remove from faction-specific entity list
	EntityList& factionList = m_entityListsByFaction[faction];
	for ( int i = 0; i < static_cast<int>( factionList.size() ); ++ i )
	{
		Entity* existingEntity = factionList[i];
		if ( existingEntity == &entity )
		{
			existingEntity = nullptr;
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::ResolveEntityVsEntityCollision()
{
	for ( int i = 0; i < static_cast<int>( m_allEntities.size() ); ++ i )
	{
		Entity* entityA = m_allEntities[i];
		if ( entityA == nullptr ) continue;
		if ( entityA == g_game->m_player )
		{
			Player* playerA = static_cast<Player*>( entityA );
			if ( playerA->m_noClip ) continue;
		}
		for ( int j = i + 1; j < static_cast<int>( m_allEntities.size() ); ++ j )
		{
			Entity* entityB = m_allEntities[j];
			if ( entityB == nullptr ) continue;
			if ( entityB == g_game->m_player )
			{
				Player* playerB = static_cast<Player*>( entityB );
				if ( playerB->m_noClip ) continue;
			}
			Vec2 aCenter = entityA->m_position;
			Vec2 bCenter = entityB->m_position;
			float aRadius = entityA->m_physicsRadius;
			float bRadius = entityB->m_physicsRadius;
			
			// A pushes B and B pushes A
			if ( entityA->m_isPushedByEntities && entityA->m_doesPushEntities &&
				 entityB->m_isPushedByEntities && entityB->m_doesPushEntities )
			{
				// Both push out of each other
				if ( PushDiscsOutOfEachOther2D( aCenter, aRadius, bCenter, bRadius ) )
				{
					entityA->m_position = aCenter;
					entityB->m_position = bCenter;
				}
				continue;
			}

			// A is fixed, A pushes B out of A
			if ( !entityA->m_isPushedByEntities && entityA->m_doesPushEntities && entityB->m_isPushedByEntities)
			{
				if ( PushDiscOutOfFixedDisc2D( bCenter, bRadius, aCenter, aRadius ) )
				{
					entityB->m_position = bCenter;
				}
				continue;
			}

			// B is fixed, B pushes A out of B
			if ( entityA->m_isPushedByEntities && !entityB->m_isPushedByEntities && entityB->m_doesPushEntities )
			{
				if ( PushDiscOutOfFixedDisc2D( aCenter, aRadius, bCenter, bRadius ) )
				{
					entityA->m_position = aCenter;
				}
				continue;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::ResolveEntityVsTileCollision()
{
	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++ entityIndex )
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity == nullptr ) continue;
		if ( !entity->m_isPushedByWalls ) continue;
		if ( entity == g_game->m_player )
		{
			Player* player = static_cast<Player*>( entity );
			if ( player->m_noClip ) continue;
		}
		IntVec2 entityTileCoords = GetTileCoordsForWorldPosition( entity->m_position );
		
		IntVec2 neighboringOffsets[] = {
			IntVec2( -1,  0 ), IntVec2(  1, 0 ), IntVec2( 0, -1 ), IntVec2( 0, 1 ), // Cardinal neighbors
			IntVec2( -1, -1 ), IntVec2( -1, 1 ), IntVec2( 1, -1 ), IntVec2( 1, 1 )  // Diagonal neighbors
		};

		// Resolve cardinal neighbors first
		for ( int offsetIndex = 0; offsetIndex < 4; ++ offsetIndex )
		{
			IntVec2 neighborCoords = entityTileCoords + neighboringOffsets[offsetIndex];
			Tile* tile = GetTile( neighborCoords );
			if ( tile != nullptr && IsTileSolid( *tile ) )
			{
				if ( PushDiscOutOfFixedAABB2D( entity->m_position, entity->m_physicsRadius, GetTileBounds( neighborCoords ) ) )
				{
					entity->m_position = entity->m_position;
				}
			}
		}

		// Then resolve diagonal neighbors
		for ( int offsetIndex = 4; offsetIndex < 8; ++ offsetIndex )
		{
			IntVec2 neighborCoords = entityTileCoords + neighboringOffsets[offsetIndex];
			Tile* tile = GetTile( neighborCoords );
			if ( tile != nullptr && IsTileSolid( *tile ) )
			{
				if ( PushDiscOutOfFixedAABB2D( entity->m_position, entity->m_physicsRadius, GetTileBounds( neighborCoords ) ) )
				{
					entity->m_position = entity->m_position;
				}
			}
		}
	}
}