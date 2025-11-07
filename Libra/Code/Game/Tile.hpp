#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
enum TileType
{
	TILE_TYPE_INVALID = -1,
	TILE_TYPE_GRASS,
	TILE_TYPE_STONE,
	NUM_TILE_TYPES
};


//-----------------------------------------------------------------------------------------------
struct TileDefinition;


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	IntVec2		m_tileCoords;
	TileType	m_type;

public:
	Tile( IntVec2 tileCoords, TileType type );
	~Tile();

	TileDefinition const& GetDefinition() const;
	bool IsSolid() const;
};