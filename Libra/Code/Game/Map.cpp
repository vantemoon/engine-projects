#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
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
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	// Do nothing
}