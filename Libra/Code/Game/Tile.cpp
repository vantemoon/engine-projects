#include "Game//Tile.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( IntVec2 tileCoords, TileType type )
	: m_tileCoords( tileCoords )
	, m_type( type )
{
}


//-----------------------------------------------------------------------------------------------
Tile::~Tile()
{
	// DO NOTHING
}