#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
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
    Vec2 camCenter;
    if ( g_game->m_player != nullptr && !g_game->m_player->m_isGarbage )
        camCenter = g_game->m_player->m_position;
    else
        camCenter = Vec2( WORLD_CENTER_X, WORLD_CENTER_Y );

    float viewHeight = static_cast<float>( g_game->m_numTilesInViewVertically ) * TILE_SIZE;
    float aspect = g_engine->m_window->m_config.m_clientAspect;
    float viewWidth = viewHeight * aspect;

    Vec2 halfExtents( viewWidth * 0.5f, viewHeight * 0.5f );

    float mapMinX = 0.f;
    float mapMinY = 0.f;
    float mapMaxX = static_cast<float>( m_dimensions.x ) * TILE_SIZE;
    float mapMaxY = static_cast<float>( m_dimensions.y ) * TILE_SIZE;

    float minCamX = mapMinX + halfExtents.x;
    float maxCamX = mapMaxX - halfExtents.x;
    float minCamY = mapMinY + halfExtents.y;
    float maxCamY = mapMaxY - halfExtents.y;

    if ( viewWidth >= ( mapMaxX - mapMinX ) ) 
	{
        camCenter.x = ( mapMinX + mapMaxX ) * 0.5f;
    } 
	else 
	{
        if ( camCenter.x < minCamX ) camCenter.x = minCamX;
        if ( camCenter.x > maxCamX ) camCenter.x = maxCamX;
    }
    if ( viewHeight >= ( mapMaxY - mapMinY ) ) 
	{
        camCenter.y = ( mapMinY + mapMaxY ) * 0.5f;
    } 
	else 
	{
        if ( camCenter.y < minCamY ) camCenter.y = minCamY;
        if ( camCenter.y > maxCamY ) camCenter.y = maxCamY;
    }

    Vec2 bottomLeft = camCenter - halfExtents;
    Vec2 topRight = camCenter + halfExtents;

    g_game->m_worldCamera->SetOrthoView( bottomLeft, topRight );

    g_engine->m_renderer->BeginCamera( *g_game->m_worldCamera );

    RenderTiles();
    RenderEntities();

    if ( g_game->m_isDebugOn ) {
        DebugRender();
    }

    g_engine->m_renderer->EndCamera( *g_game->m_worldCamera );
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
		if ( tile.m_type == TileType::GRASS )
		{
			tileColor = GRASS_COLOR;
		}
		else if ( tile.m_type == TileType::STONE )
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
	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++ entityIndex )
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity != nullptr )
		{
			DebugDrawRing( entity->m_position, entity->m_physicsRadius, 0.1f, Rgba8( 255, 0, 0 ) );
			DebugDrawRing( entity->m_position, entity->m_cosmeticRadius, 0.1f, Rgba8( 0, 0, 255 ) );
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
				type = TileType::STONE;
			}
			else
			{
				if ( isInnerReserved )
				{
					if ( ( x == 2 && y == 4 ) || ( x == 3 && y == 4 ) || ( x == 4 && y == 4 ) ||
						( x == 4 && y == 3 ) || ( x == 4 && y == 2 ) )
					{
						type = TileType::STONE;
					}
					else
					{
						type = TileType::GRASS;
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
						type = TileType::STONE;
					}
					else
					{
						type = TileType::GRASS;
					}
				}
				else
				{
					RandomNumberGenerator rng;
					float roll = rng.RollRandomFloatZeroToOne();
					if ( roll < INNER_STONE_TILE_PROBABILITY )
					{
						type = TileType::STONE;
					}
					else
					{
						type = TileType::GRASS;
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