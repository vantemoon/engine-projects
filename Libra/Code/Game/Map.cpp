#include "Game/Map.hpp"
#include "Game/Aries.hpp"
#include "Game/Bullet.hpp"
#include "Game/Explosion.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Leo.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/Scorpio.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( MapDefinition const* definition, int index )
	: m_definition( definition )
	, m_dimensions( definition->m_dimensions )
	, m_index( index )
{
	PopulateMap();
	SpawnEntitiesForMapDefinition();

	// Add player to the map
	if ( g_game->m_player != nullptr )
	{
		AddEntityToMap( *g_game->m_player, ENTITY_TYPE_GOOD_PLAYER, ENTITY_FACTION_GOOD );
	}

	m_discoverySoundCooldown = g_gameConfigBlackboard.GetValue( "discoverySoundCooldown", 0.1f );
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	// Do nothing
}


//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds )
{
	m_timeSinceLastDiscoverySound += deltaSeconds;

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

	if (g_game->m_isDebugOn) 
	{
		DebugRender();
	}

	RenderEntities();
}


//-----------------------------------------------------------------------------------------------
void Map::RenderTiles() const
{
	std::vector<Vertex> tileVerts;

	g_engine->m_renderer->BindTexture( &g_game->m_tileSpriteSheet->GetTexture() );

	for ( Tile const& tile : m_tiles )
	{
		float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );
		AABB2 tileBounds( tile.m_tileCoords.x * tileSize, tile.m_tileCoords.y * tileSize,
						( tile.m_tileCoords.x + 1 ) * tileSize, ( tile.m_tileCoords.y + 1 ) * tileSize );

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
		Explosion* explosion = dynamic_cast<Explosion*>( entity );
		if ( explosion != nullptr )
		{
			// Explosions are rendered last, after other entities
			continue;
		}
		if ( entity != nullptr )
		{
			entity->Render();
		}
	}

	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_entityListsByType[ENTITY_TYPE_EXPLOSION].size() ); ++ entityIndex )
	{
		Entity* entity = m_entityListsByType[ENTITY_TYPE_EXPLOSION][entityIndex];
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

	switch ( g_game->m_mapRenderMode )
	{
		case 0:
			break;

		case 1:
		{
			TileHeatMap dijkstraMap( m_dimensions );
			PopulateDijkstraMap(
				dijkstraMap,
				IntVec2( 1, 1 ),
				999999.f,
				true
			);

			float maxCost = 0.f;
			for ( int y = 0; y < m_dimensions.y; ++y )
			{
				for ( int x = 0; x < m_dimensions.x; ++x )
				{
					float v = dijkstraMap.GetValueAtTileCoords( IntVec2( x, y ) );
					if ( v < 999999.f && v > maxCost )
					{
						maxCost = v;
					}
				}
			}

			if ( maxCost <= 0.f )
			{
				maxCost = 1.f;
			}

			std::vector<Vertex> debugVerts;
			dijkstraMap.AddVertsForDebugDraw(
				debugVerts,
				AABB2(
					Vec2::ZERO,
					Vec2(
						static_cast< float >( m_dimensions.x ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f ),
						static_cast< float >( m_dimensions.y ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f )
					)
				),
				FloatRange( 0.f, maxCost ),
				Rgba8::BLACK,
				Rgba8::WHITE,
				999999.f,
				Rgba8::BLUE
			);

			g_engine->m_renderer->DrawVertexArray( static_cast<int>( debugVerts.size() ), debugVerts.data() );
		}
		break;

		case 2:
		{
			if ( m_entityListsByType[ENTITY_TYPE_EVIL_LEO].size() == 0 ) return;

			Leo* leo = dynamic_cast< Leo* >( m_entityListsByType[ENTITY_TYPE_EVIL_LEO][0] );
			if ( leo == nullptr ) return;

			TileHeatMap reachabilityMap( m_dimensions );
			PopulateEnemyReachabilityMap( 
				reachabilityMap, 
				GetTileCoordsForWorldPosition( leo->m_position ),
				999999.f,
				true
			);

			std::vector<Vertex> debugVerts;
			reachabilityMap.AddVertsForDebugDraw(
				debugVerts,
				AABB2(
					Vec2::ZERO,
					Vec2(
						static_cast< float >( m_dimensions.x ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f ),
						static_cast< float >( m_dimensions.y ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f )
					)
				),
				FloatRange( 0.f, 1.f ),
				Rgba8::WHITE,
				Rgba8::BLACK,
				999999.f,
				Rgba8::BLUE
			);

			g_engine->m_renderer->DrawVertexArray( static_cast<int>( debugVerts.size() ), debugVerts.data() );
		}
		break;

		case 3:
		{
			if ( m_entityListsByType[ENTITY_TYPE_EVIL_LEO].size() == 0 ) return;

			Leo* leo = dynamic_cast< Leo* >( m_entityListsByType[ENTITY_TYPE_EVIL_LEO][0] );
			if ( leo == nullptr ) return;

			TileHeatMap dijkstraMap( m_dimensions );
			PopulateDijkstraMap(
				dijkstraMap,
				GetTileCoordsForWorldPosition( leo->m_goalPosition ),
				999999.f,
				true
			);

			float maxCost = 0.f;
			for ( int y = 0; y < m_dimensions.y; ++y )
			{
				for ( int x = 0; x < m_dimensions.x; ++x )
				{
					float v = dijkstraMap.GetValueAtTileCoords( IntVec2( x, y ) );
					if ( v < 999999.f && v > maxCost )
					{
						maxCost = v;
					}
				}
			}

			if ( maxCost <= 0.f )
			{
				maxCost = 1.f;
			}

			std::vector<Vertex> debugVerts;
			dijkstraMap.AddVertsForDebugDraw(
				debugVerts,
				AABB2(
					Vec2::ZERO,
					Vec2(
						static_cast< float >( m_dimensions.x ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f ),
						static_cast< float >( m_dimensions.y ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f )
					)
				),
				FloatRange( 0.f, maxCost ),
				Rgba8::BLACK,
				Rgba8::WHITE,
				999999.f,
				Rgba8::BLUE
			);

			g_engine->m_renderer->DrawVertexArray( static_cast< int >( debugVerts.size() ), debugVerts.data() );
		}
		break;

		default:
			break;
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
	float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

	float minX = static_cast<float>( tileCoords.x ) * tileSize;
	float minY = static_cast<float>( tileCoords.y ) * tileSize;
	float maxX = static_cast<float>( tileCoords.x + 1 ) * tileSize;
	float maxY = static_cast<float>( tileCoords.y + 1 ) * tileSize;

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
	float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

	int tileX = static_cast<int>( worldPos.x / tileSize );
	int tileY = static_cast<int>( worldPos.y / tileSize );

	return IntVec2( tileX, tileY );
}


//-----------------------------------------------------------------------------------------------
Vec2 Map::GetWorldPositionForTileCoords( IntVec2 tileCoords ) const
{
	float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

	float worldX = ( static_cast<float>( tileCoords.x ) + 0.5f ) * tileSize;
	float worldY = ( static_cast<float>( tileCoords.y ) + 0.5f ) * tileSize;

	return Vec2( worldX, worldY );
}


//-----------------------------------------------------------------------------------------------
void Map::GenerateTiles()
{
	static bool s_initialized = false;
	if ( !s_initialized )
	{
		TileDefinition::InitializeTileDefinitions();
		s_initialized = true;
	}

	m_tiles.clear();

	RandomNumberGenerator rng;
	rng.SetSeed( static_cast<unsigned int>( GetCurrentTimeSeconds() * 100000.0 ) );


	for ( int y = 0; y < m_dimensions.y; ++y )
	{
		for ( int x = 0; x < m_dimensions.x; ++x )
		{
			IntVec2 tileCoords = IntVec2( x, y );
			TileDefinition const* type = nullptr;

			bool isOnEdge = ( x == 0 || y == 0 || x == m_dimensions.x - 1 || y == m_dimensions.y - 1 );
			bool isInnerReserved = ( x >= 1 && x <= 5 && y >= 1 && y <= 5 );
			bool isOuterReserved = ( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 && y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 );
			if ( isOnEdge )
			{
				type = m_definition->m_borderTileType;
			}
			else
			{
				if ( isInnerReserved )
				{
					if ( x == 1 && y == 1 )
					{
						type = TileDefinition::s_definitions["Entrance"];
					}
					else if ( ( x == 2 && y == 4 ) || ( x == 3 && y == 4 ) || ( x == 4 && y == 4 ) ||
						( x == 4 && y == 3 ) || ( x == 4 && y == 2 ) )
					{
						type = m_definition->m_startbunkerWallTileType;
					}
					else
					{
						type = m_definition->m_bunkerFloorTileType;
					}
				}
				else if ( isOuterReserved )
				{
					if ( x == m_dimensions.x - 2 && y == m_dimensions.y - 2 )
					{
						type = TileDefinition::s_definitions["Exit"];
					}
					else if ( ( x == m_dimensions.x - 6 && y == m_dimensions.y - 3 ) ||
						( x == m_dimensions.x - 6 && y == m_dimensions.y - 4 ) ||
						( x == m_dimensions.x - 6 && y == m_dimensions.y - 5 ) ||
						( x == m_dimensions.x - 6 && y == m_dimensions.y - 6 ) ||
						( x == m_dimensions.x - 5 && y == m_dimensions.y - 6 ) ||
						( x == m_dimensions.x - 4 && y == m_dimensions.y - 6 ) ||
						( x == m_dimensions.x - 3 && y == m_dimensions.y - 6 ) )
					{
						type = m_definition->m_exitbunkerWallTileType;
					}
					else
					{
						type = m_definition->m_bunkerFloorTileType;
					}
				}
				else
				{
					type = m_definition->m_fillTileType;
				}
			}
			ASSERT_OR_DIE( type != nullptr, "TileDefinition pointer is null!" );

			Tile newTile = Tile( tileCoords, type );
			m_tiles.push_back( newTile );
		}
	}

	// Worm 1
	for ( int wormIndex = 0; wormIndex < m_definition->m_numOfWorms1; ++ wormIndex )
	{


		IntVec2 wormStart;
		while ( true )
		{
			int x = rng.RollRandomIntInRange( 1, m_dimensions.x - 2 );
			int y = rng.RollRandomIntInRange( 1, m_dimensions.y - 2 );
			wormStart = IntVec2( x, y );
			bool isInReservedArea = ( ( x >= 1 && x <= 5 && y >= 1 && y <= 5 ) ||
				( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 &&
					y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 ) );
			if ( !isInReservedArea )
			{
				break;
			}
		}

		IntVec2 currentPos = wormStart;
		TileDefinition const* wormTileType = m_definition->m_worm1TileType;

		int tileIndex = currentPos.y * m_dimensions.x + currentPos.x;
		m_tiles[tileIndex] = Tile( currentPos, wormTileType );

		for ( int segmentIndex = 1; segmentIndex < m_definition->m_wormLength1; ++ segmentIndex )
		{
			std::vector<IntVec2> possibleDirections;
			possibleDirections.push_back( IntVec2( 1, 0 ) );
			possibleDirections.push_back( IntVec2( -1, 0 ) );
			possibleDirections.push_back( IntVec2( 0, 1 ) );
			possibleDirections.push_back( IntVec2( 0, -1 ) );

			int randomDirIndex = rng.RollRandomIntInRange( 0, static_cast< int >( possibleDirections.size() ) - 1 );
			IntVec2 chosenDirection = possibleDirections[randomDirIndex];
			currentPos = currentPos + chosenDirection;
			if ( !IsTileCoordsInBounds( currentPos ) )
			{
				break;
			}
			int x = currentPos.x;
			int y = currentPos.y;

			bool isOnEdge = ( x == 0 || y == 0 || x == m_dimensions.x - 1 || y == m_dimensions.y - 1 );
			bool isInnerReserved = ( x >= 1 && x <= 5 && y >= 1 && y <= 5 );
			bool isOuterReserved = ( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 && y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 );
			if ( isOnEdge || isInnerReserved || isOuterReserved )
			{
				break;
			}
			int index = currentPos.y * m_dimensions.x + currentPos.x;
			m_tiles[index] = Tile( currentPos, wormTileType );
		}
	}

	// Worm 2
	for ( int wormIndex = 0; wormIndex < m_definition->m_numOfWorms2; ++ wormIndex )
	{
		IntVec2 wormStart;

		while ( true )
		{
			int x = rng.RollRandomIntInRange( 1, m_dimensions.x - 2 );
			int y = rng.RollRandomIntInRange( 1, m_dimensions.y - 2 );
			wormStart = IntVec2( x, y );
			bool isInReservedArea = ( ( x >= 1 && x <= 5 && y >= 1 && y <= 5 ) ||
				( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 &&
					y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 ) );
			if ( !isInReservedArea )
			{
				break;
			}
		}

		IntVec2 currentPos = wormStart;
		TileDefinition const* wormTileType = m_definition->m_worm2TileType;

		int tileIndex = currentPos.y * m_dimensions.x + currentPos.x;
		m_tiles[tileIndex] = Tile( currentPos, wormTileType );

		for ( int segmentIndex = 1; segmentIndex < m_definition->m_wormLength2; ++ segmentIndex )
		{
			std::vector<IntVec2> possibleDirections;
			possibleDirections.push_back( IntVec2( 1, 0 ) );
			possibleDirections.push_back( IntVec2( -1, 0 ) );
			possibleDirections.push_back( IntVec2( 0, 1 ) );
			possibleDirections.push_back( IntVec2( 0, -1 ) );
			int randomDirIndex = rng.RollRandomIntInRange( 0, static_cast< int >( possibleDirections.size() ) - 1 );
			IntVec2 chosenDirection = possibleDirections[randomDirIndex];
			currentPos = currentPos + chosenDirection;
			if ( !IsTileCoordsInBounds( currentPos ) )
			{
				break;
			}
			int x = currentPos.x;
			int y = currentPos.y;
			bool isOnEdge = ( x == 0 || y == 0 || x == m_dimensions.x - 1 || y == m_dimensions.y - 1 );
			bool isInnerReserved = ( x >= 1 && x <= 5 && y >= 1 && y <= 5 );
			bool isOuterReserved = ( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 && y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 );
			if ( isOnEdge || isInnerReserved || isOuterReserved )
			{
				break;
			}
			int index = currentPos.y * m_dimensions.x + currentPos.x;
			m_tiles[index] = Tile( currentPos, wormTileType );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::GenerateTilesFromImage()
{
	Image mapImage = Image( m_definition->m_mapImageName.c_str() );

	IntVec2 imageDimensions = mapImage.GetDimensions();
	if ( imageDimensions.x != m_dimensions.x || imageDimensions.y != m_dimensions.y )
	{
		ERROR_RECOVERABLE( "Map image dimensions do not match map dimensions." );
		return;
	}

	RandomNumberGenerator rng;

	for ( int y = 0; y < m_dimensions.y; ++y )
	{
		for ( int x = 0; x < m_dimensions.x; ++x )
		{
			IntVec2 tileCoords = IntVec2( x, y );

			if ( x >= imageDimensions.x || y >= imageDimensions.y )
			{
				continue;
			}

			Rgba8 pixelColor = mapImage.GetTexelColor( tileCoords );

			if ( pixelColor.a == 0 )
			{
				continue;
			}

			TileDefinition const* matchedDef = nullptr;

			for ( auto const& pair : TileDefinition::s_definitions )
			{
				TileDefinition const* tileDef = pair.second;
				if ( tileDef == nullptr )
				{
					continue;
				}

				if ( tileDef->m_mapImageColor.r == pixelColor.r &&
					tileDef->m_mapImageColor.g == pixelColor.g &&
					tileDef->m_mapImageColor.b == pixelColor.b )
				{
					matchedDef = tileDef;
					break;
				}
			}

			if ( matchedDef == nullptr )
			{
				ERROR_RECOVERABLE( "No matching tile definition found for pixel color in map image." );
				continue;
			}

			int roll = rng.RollRandomIntInRange( 0, 255 );
			if ( roll <= static_cast< int >( pixelColor.a ) )
			{
				int tileIndex = y * m_dimensions.x + x;
				m_tiles[tileIndex] = Tile( tileCoords, matchedDef );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::PopulateMap()
{
	for ( int attempt = 0; attempt < g_gameConfigBlackboard.GetValue( "maxGenerationAttemps", 1000 ); ++attempt )
	{
		m_tiles.clear();
		bool hasImage = m_definition->m_mapImageName != "";
		GenerateTiles();
		if ( hasImage )
		{
			GenerateTilesFromImage();
		}
		if ( IsMapValid() )
		{
			FillUnreachableTiles();
			return;
		}
	}
	ERROR_AND_DIE( "Map generation failed: could not generate a valid map in the given number of attempts." );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsMapValid() const
{
	TileHeatMap dijkstraMap( m_dimensions );
	PopulateDijkstraMap(
		dijkstraMap,
		GetTileCoordsForWorldPosition( GetWorldPositionForTileCoords( IntVec2( 1, 1 ) ) ),
		999999.f,
		true
	);
	IntVec2 exitCoords = IntVec2( m_dimensions.x - 2, m_dimensions.y - 2 );
	float exitCost = dijkstraMap.GetValueAtTileCoords( exitCoords );
	if ( exitCost >= 999999.f )
	{
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------
void Map::FillUnreachableTiles()
{
	TileHeatMap dijkstraMap( m_dimensions );
	PopulateDijkstraMap(
		dijkstraMap,
		GetTileCoordsForWorldPosition( GetWorldPositionForTileCoords( IntVec2( 1, 1 ) ) ),
		999999.f,
		false
	);
	for ( int y = 0; y < m_dimensions.y; ++y )
	{
		for ( int x = 0; x < m_dimensions.x; ++x )
		{
			bool isOnEdge = ( x == 0 || y == 0 || x == m_dimensions.x - 1 || y == m_dimensions.y - 1 );
			bool isInnerReserved = ( x >= 1 && x <= 5 && y >= 1 && y <= 5 );
			bool isOuterReserved = ( x >= m_dimensions.x - 7 && x <= m_dimensions.x - 2 && y >= m_dimensions.y - 7 && y <= m_dimensions.y - 2 );
			if ( isOnEdge || isInnerReserved || isOuterReserved )
			{
				continue;
			}
			IntVec2 tileCoords = IntVec2( x, y );
			float cost = dijkstraMap.GetValueAtTileCoords( tileCoords );
			if ( cost >= 999999.f && !IsTileSolid( *GetTile( tileCoords ), false ) )
			{
				int tileIndex = tileCoords.y * m_dimensions.x + tileCoords.x;
				m_tiles[tileIndex] = Tile( tileCoords, m_definition->m_worm2TileType );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DeleteGarbageEntities()
{
	for ( int typeIndex = 0; typeIndex < NUM_ENTITY_TYPES; ++ typeIndex )
	{
		std::vector<Entity*>& entityList = m_entityListsByType[typeIndex];
		EntityType entityType = static_cast<EntityType>( typeIndex );
		for ( int entityIndex = static_cast<int>( entityList.size() ) - 1; entityIndex >= 0; -- entityIndex )
		{
			Entity* entity = entityList[entityIndex];
			if ( entity != nullptr && entity->m_isGarbage )
			{
				RemoveEntityFromMap( *entity, entityType, entity->m_faction );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateWorldCameraView() const
{
	float tilesInViewY = static_cast< float >( g_game->m_numTilesInViewVertically );
	float aspect = g_engine->m_window->m_config.m_clientAspect;
	float viewHeight = tilesInViewY * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );
	float viewWidth = viewHeight * aspect;

	float mapWidth = static_cast< float >( m_dimensions.x ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );
	float mapHeight = static_cast< float >( m_dimensions.y ) * g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

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
	return IsTileSolid( *tile, false );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileSolid( Tile tile, bool treatWaterAsSolid ) const
{
	if ( tile.GetDefinition().m_isSolid )
	{
		return true;
	}
	if ( treatWaterAsSolid && tile.GetDefinition().m_isWater )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileOpaque( Tile tile ) const
{
	return IsTileSolid( tile, false );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileBlockedForDijkstra( IntVec2 const& coords, bool treatWaterAsSolid,
									std::vector<bool> const& scorpioBlocked ) const
{
	if ( !IsTileCoordsInBounds( coords ) )
	{
		return true;
	}

	int index = coords.x + coords.y * m_dimensions.x;

	if ( scorpioBlocked[index] )
	{
		return true;
	}

	Tile const* tile = GetTile( coords );
	if ( tile == nullptr )
	{
		return true;
	}
	if ( IsTileSolid( *tile, treatWaterAsSolid ) )
	{
		return true;
	}
	if ( treatWaterAsSolid && tile->GetDefinition().m_isWater )
	{
		return true;
	}

	return false;
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
		if ( tile != nullptr && IsTileOpaque( *tile ) )
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
//-----------------------------------------------------------------------------------------------
void Map::SpawnEntitiesForMapDefinition()
{
	MapDefinition const& mapDef = *m_definition;

	RandomNumberGenerator rng;
	rng.SetSeed( static_cast< unsigned int >( GetCurrentTimeSeconds() * 100000.0 ) + static_cast< unsigned int >( m_index ) * 977u );

	float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

	// Spawn Scorpios
	for ( int i = 0; i < mapDef.m_numOfScorpios; ++i )
	{
		Vec2   rawSpawnPos;
		IntVec2 spawnTileCoords;
		bool   isRespawnPosValid = false;

		while ( !isRespawnPosValid )
		{
			rawSpawnPos.x = rng.RollRandomFloatInRange( 2.f * tileSize, ( static_cast< float >( m_dimensions.x ) - 3.f ) * tileSize );
			rawSpawnPos.y = rng.RollRandomFloatInRange( 2.f * tileSize, ( static_cast< float >( m_dimensions.y ) - 3.f ) * tileSize );

			spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
			isRespawnPosValid = true;

			// Inner bunker area
			if ( spawnTileCoords.x >= 1 && spawnTileCoords.x <= 6 &&
				spawnTileCoords.y >= 1 && spawnTileCoords.y <= 6 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Outer bunker area
			if ( spawnTileCoords.x >= m_dimensions.x - 7 && spawnTileCoords.x <= m_dimensions.x - 2 &&
				spawnTileCoords.y >= m_dimensions.y - 7 && spawnTileCoords.y <= m_dimensions.y - 2 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Solid tiles
			Tile* spawnTile = GetTile( spawnTileCoords );
			bool scorpionCanSwim = g_gameConfigBlackboard.GetValue( "scorpionCanSwim", false );
			if ( spawnTile == nullptr || IsTileSolid( *spawnTile, !scorpionCanSwim ) )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Occupied tiles
			if ( IsAnyEntityAtTile( spawnTileCoords ) )
			{
				isRespawnPosValid = false;
				continue;
			}
		}

		Vec2 spawnPos = GetWorldPositionForTileCoords( spawnTileCoords );

		float orientation = rng.RollRandomFloatInRange( 0.f, 360.f );
		Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EVIL_SCORPIO, spawnPos, orientation );
		if ( newEntity != nullptr )
		{
			AddEntityToMap( *newEntity, ENTITY_TYPE_EVIL_SCORPIO, ENTITY_FACTION_EVIL );
		}
	}

	// Spawn Leos
	for ( int i = 0; i < mapDef.m_numOfLeos; ++i )
	{
		Vec2   rawSpawnPos;
		IntVec2 spawnTileCoords;
		bool   isRespawnPosValid = false;

		while ( !isRespawnPosValid )
		{
			rawSpawnPos.x = rng.RollRandomFloatInRange( 2.f * tileSize, ( static_cast< float >( m_dimensions.x ) - 3.f ) * tileSize );
			rawSpawnPos.y = rng.RollRandomFloatInRange( 2.f * tileSize, ( static_cast< float >( m_dimensions.y ) - 3.f ) * tileSize );

			spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
			isRespawnPosValid = true;

			// Inner bunker area
			if ( spawnTileCoords.x >= 1 && spawnTileCoords.x <= 6 &&
				spawnTileCoords.y >= 1 && spawnTileCoords.y <= 6 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Outer bunker area
			if ( spawnTileCoords.x >= m_dimensions.x - 7 && spawnTileCoords.x <= m_dimensions.x - 2 &&
				spawnTileCoords.y >= m_dimensions.y - 7 && spawnTileCoords.y <= m_dimensions.y - 2 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Solid tiles
			Tile* spawnTile = GetTile( spawnTileCoords );
			bool leoCanSwim = g_gameConfigBlackboard.GetValue( "leoCanSwim", true );
			if ( spawnTile == nullptr || IsTileSolid( *spawnTile, !leoCanSwim ) )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Occupied tiles
			if ( IsAnyEntityAtTile( spawnTileCoords ) )
			{
				isRespawnPosValid = false;
				continue;
			}
		}

		Vec2 spawnPos = GetWorldPositionForTileCoords( spawnTileCoords );

		float orientation = rng.RollRandomFloatInRange( 0.f, 360.f );
		Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EVIL_LEO, spawnPos, orientation );
		if ( newEntity != nullptr )
		{
			AddEntityToMap( *newEntity, ENTITY_TYPE_EVIL_LEO, ENTITY_FACTION_EVIL );
		}
	}

	// Spawn Aries
	for ( int i = 0; i < mapDef.m_numOfAries; ++i )
	{
		Vec2   rawSpawnPos;
		IntVec2 spawnTileCoords;
		bool   isRespawnPosValid = false;

		while ( !isRespawnPosValid )
		{
			rawSpawnPos.x = rng.RollRandomFloatInRange( 2.f * tileSize, ( static_cast< float >( m_dimensions.x ) - 3.f ) * tileSize );
			rawSpawnPos.y = rng.RollRandomFloatInRange( 2.f * tileSize, ( static_cast< float >( m_dimensions.y ) - 3.f ) * tileSize );

			spawnTileCoords = GetTileCoordsForWorldPosition( rawSpawnPos );
			isRespawnPosValid = true;

			// Inner bunker area
			if ( spawnTileCoords.x >= 1 && spawnTileCoords.x <= 6 &&
				spawnTileCoords.y >= 1 && spawnTileCoords.y <= 6 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Outer bunker area
			if ( spawnTileCoords.x >= m_dimensions.x - 7 && spawnTileCoords.x <= m_dimensions.x - 2 &&
				spawnTileCoords.y >= m_dimensions.y - 7 && spawnTileCoords.y <= m_dimensions.y - 2 )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Solid tiles
			Tile* spawnTile = GetTile( spawnTileCoords );
			bool ariesCanSwim = g_gameConfigBlackboard.GetValue( "ariesCanSwim", true );
			if ( spawnTile == nullptr || IsTileSolid( *spawnTile, !ariesCanSwim ) )
			{
				isRespawnPosValid = false;
				continue;
			}

			// Occupied tiles
			if ( IsAnyEntityAtTile( spawnTileCoords ) )
			{
				isRespawnPosValid = false;
				continue;
			}
		}

		Vec2 spawnPos = GetWorldPositionForTileCoords( spawnTileCoords );

		float orientation = rng.RollRandomFloatInRange( 0.f, 360.f );
		Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EVIL_ARIES, spawnPos, orientation );
		if ( newEntity != nullptr )
		{
			AddEntityToMap( *newEntity, ENTITY_TYPE_EVIL_ARIES, ENTITY_FACTION_EVIL );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnExplosionAtPosition( Vec2 const& position, float duration /*= 0.5f*/, float scale /*= 1.f */, bool orientRandomly /*= true */, float orientationDegrees /*= 0.f */ )
{
	RandomNumberGenerator rng;
	float orientation = 0.f;
	if ( orientRandomly ) orientation = rng.RollRandomFloatInRange( 0.f, 360.f );
	else orientation = orientationDegrees;

	Entity* newEntity = SpawnNewEntity( ENTITY_TYPE_EXPLOSION, position, orientation, duration, scale );
	if ( newEntity != nullptr )
	{
		AddEntityToMap( *newEntity, ENTITY_TYPE_EXPLOSION, ENTITY_FACTION_NEUTRAL );
	}
}


//-----------------------------------------------------------------------------------------------
Entity* Map::SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees, float duration /*= 0.5f*/, float scale /*= 1.f */ )
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
	else if ( type == ENTITY_TYPE_GOOD_FLAME_BULLET )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_GOOD_FLAME_BULLET );
	}
	else if ( type == ENTITY_TYPE_EVIL_BULLET )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_EVIL_BULLET );
	}
	else if ( type == ENTITY_TYPE_EVIL_BOLT )
	{
		newEntity = new Bullet( position, orientationDegrees, ENTITY_TYPE_EVIL_BOLT );
	}
	else if ( type == ENTITY_TYPE_EXPLOSION )
	{
		newEntity = new Explosion( position, orientationDegrees, duration, scale);
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
		if ( m_allEntities[i] == &entity )
		{
			m_allEntities[i] = nullptr;
			break;
		}
	}

	// Remove from type-specific entity list
	EntityList& typeList = m_entityListsByType[type];
	for ( int i = 0; i < static_cast<int>( typeList.size() ); ++ i )
	{
		if ( typeList[i] == &entity )
		{
			typeList[i] = nullptr;
			break;
		}
	}

	// Remove from faction-specific entity list
	EntityList& factionList = m_entityListsByFaction[faction];
	for ( int i = 0; i < static_cast<int>( factionList.size() ); ++ i )
	{
		if ( factionList[i] == &entity )
		{
			factionList[i] = nullptr;
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

		bool canEntitySwim = entity->m_canSwim;

		if ( !entity->m_isPushedByWalls ) continue;
		if ( entity == g_game->m_player )
		{
			Player* player = static_cast<Player*>( entity );
			if ( player->m_noClip ) continue;
		}
		IntVec2 entityTileCoords = GetTileCoordsForWorldPosition( entity->m_position );

		IntVec2 neighboringOffsets[] = {
			IntVec2( -1,  0 ), IntVec2(  1, 0 ), IntVec2( 0, -1 ), IntVec2( 0, 1 ),
			IntVec2( -1, -1 ), IntVec2( -1, 1 ), IntVec2( 1, -1 ), IntVec2( 1, 1 )
		};

		// Resolve cardinal neighbors first
		for ( int offsetIndex = 0; offsetIndex < 4; ++ offsetIndex )
		{
			IntVec2 neighborCoords = entityTileCoords + neighboringOffsets[offsetIndex];
			Tile* tile = GetTile( neighborCoords );
			if ( tile != nullptr && IsTileSolid( *tile, !canEntitySwim ) )
			{
				PushDiscOutOfFixedAABB2D( entity->m_position, entity->m_physicsRadius, GetTileBounds( neighborCoords ) );
			}
		}

		// Then resolve diagonal neighbors
		for ( int offsetIndex = 4; offsetIndex < 8; ++ offsetIndex )
		{
			IntVec2 neighborCoords = entityTileCoords + neighboringOffsets[offsetIndex];
			Tile* tile = GetTile( neighborCoords );
			if ( tile != nullptr && IsTileSolid( *tile, !canEntitySwim ) )
			{
				PushDiscOutOfFixedAABB2D( entity->m_position, entity->m_physicsRadius, GetTileBounds( neighborCoords ) );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::BuildScorpioBlockedMask( std::vector<bool>& out_mask ) const
{
	int tileCount = m_dimensions.x * m_dimensions.y;
	out_mask.clear();
	out_mask.resize( tileCount );

	for ( int i = 0; i < tileCount; ++i )
	{
		out_mask[i] = false;
	}

	int scorpioCount = static_cast< int >( m_entityListsByType[ENTITY_TYPE_EVIL_SCORPIO].size() );
	for ( int i = 0; i < scorpioCount; ++i )
	{
		Entity* scorpio = m_entityListsByType[ENTITY_TYPE_EVIL_SCORPIO][i];
		if ( scorpio == nullptr )
		{
			continue;
		}

		IntVec2 coords = GetTileCoordsForWorldPosition( scorpio->m_position );
		if ( IsTileCoordsInBounds( coords ) )
		{
			int index = coords.x + coords.y * m_dimensions.x;
			out_mask[index] = true; 
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::PopulateDijkstraMap( TileHeatMap& out_dijkstraMap, IntVec2 startCoords, float maxCost, bool treatWaterAsSolid /*= true*/ ) const
{
	std::vector<bool> scorpioBlocked;
	BuildScorpioBlockedMask( scorpioBlocked );

	out_dijkstraMap.SetAllValues( maxCost );

	if ( !IsTileCoordsInBounds( startCoords ) ) return;

	Tile* startTile = GetTile( startCoords );
	if ( startTile == nullptr ) return;
	if ( IsTileSolid( *startTile, treatWaterAsSolid ) ) return;
	if ( treatWaterAsSolid && startTile->GetDefinition().m_isWater ) return;

	out_dijkstraMap.SetValueAtTileCoords( startCoords, 0.0f );

	IntVec2 neighborOffsets[4] =
	{
		IntVec2( -1,  0 ),
		IntVec2( 1,  0 ),
		IntVec2( 0, -1 ),
		IntVec2( 0,  1 )
	};

	bool didUpdate = true;
	while ( didUpdate )
	{
		didUpdate = false;

		for ( int y = 0; y < m_dimensions.y; ++y )
		{
			for ( int x = 0; x < m_dimensions.x; ++x )
			{
				IntVec2 currentCoords( x, y );
				Tile* currentTile = GetTile( currentCoords );
				if ( currentTile == nullptr ) continue;
				if ( IsTileSolid( *currentTile, treatWaterAsSolid ) ) continue;
				if ( treatWaterAsSolid && currentTile->GetDefinition().m_isWater ) continue;

				float currentCost = out_dijkstraMap.GetValueAtTileCoords( currentCoords );
				if ( currentCost >= maxCost ) continue;

				for ( int i = 0; i < 4; ++i )
				{
					IntVec2 neighborCoords = currentCoords + neighborOffsets[i];
					if ( IsTileBlockedForDijkstra( neighborCoords, treatWaterAsSolid, scorpioBlocked ) ) continue;
					
					float neighborCost = out_dijkstraMap.GetValueAtTileCoords( neighborCoords );
					float newCost = currentCost + 1.0f;

					if ( newCost < neighborCost )
					{
						out_dijkstraMap.SetValueAtTileCoords( neighborCoords, newCost );
						didUpdate = true;
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::PopulateEnemyReachabilityMap( TileHeatMap& out_reachabilityMap, IntVec2 startCoords, float maxCost, bool treatWaterAsSolid /*= true*/ ) const
{
	TileHeatMap* dijkstraMap = new TileHeatMap( out_reachabilityMap.m_dimensions );
	PopulateDijkstraMap( *dijkstraMap, startCoords, maxCost, treatWaterAsSolid );

	for ( int y = 0; y < m_dimensions.y; ++y )
	{
		for ( int x = 0; x < m_dimensions.x; ++x )
		{
			IntVec2 currentCoords( x, y );
			float cost = dijkstraMap->GetValueAtTileCoords( currentCoords );

			if ( cost < maxCost )
			{
				out_reachabilityMap.SetValueAtTileCoords( currentCoords, 1.f );
			}
			else
			{
				out_reachabilityMap.SetValueAtTileCoords( currentCoords, 0.f );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Map::IsAnyEntityAtTile( IntVec2 tileCoords ) const
{
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); ++ i )
	{
		Entity* e = m_allEntities[i];
		if ( e == nullptr ) continue;

		IntVec2 eTile = GetTileCoordsForWorldPosition( e->m_position );
		if ( eTile == tileCoords )
		{
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void Map::PlayDiscoverySoundIfReady()
{
	if ( m_timeSinceLastDiscoverySound >= m_discoverySoundCooldown )
	{
		g_engine->m_audioSystem->StartSound( g_game->m_discoverySoundID );
		m_timeSinceLastDiscoverySound = 0.f;
	}
}