#pragma once
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
enum TileType
{
	GRASS,
	STONE,
	NUM_TILE_TYPES
};


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	IntVec2		m_tileCoords;
	TileType	m_type;

public:
	Tile( IntVec2 tileCoords, TileType type );
	~Tile();

	bool IsSolid() const;
};