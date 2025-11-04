#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
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

	m_allEntities.push_back( g_game->m_player );
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

	const float ringThickness = 0.7f;
	const float thinLineThickness = 0.7f;
	const float thickLineThickness = 1.5f;

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
			float turretGoalDegrees = player->m_turretTargetDegrees;
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
void Map::PopulateTiles()
{
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