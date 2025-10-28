#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( IntVec2 dimensions )
	: m_dimensions( dimensions )
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

	m_allEntities.push_back( g_game->m_player );
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	// Do nothing
}


//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	g_engine->m_renderer->BeginCamera( *g_game->m_worldCamera );

	int numTiles = static_cast<int>( m_tiles.size() );
	std::vector<Vertex> tileVerts;
	for ( int tileIndex = 0; tileIndex < numTiles; ++tileIndex )
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
	g_engine->m_renderer->DrawVertexArray( static_cast<int>( tileVerts.size() ), tileVerts.data() );

	for ( int entityIndex = 0; entityIndex < static_cast<int>( m_allEntities.size() ); ++entityIndex )
	{
		Entity* entity = m_allEntities[entityIndex];
		if ( entity != nullptr )
		{
			entity->Render();
		}
	}

	g_engine->m_renderer->EndCamera( *g_game->m_worldCamera );
}