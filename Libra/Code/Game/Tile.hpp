#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
enum TileType
{
	TILE_TYPE_INVALID = -1,

	// Row 1
	TILE_TYPE_GRASS_1,
	TILE_TYPE_GRASS_2,
	TILE_TYPE_GRASS_3,
	TILE_TYPE_GRASS_4,
	TILE_TYPE_GRASS_5,
	TILE_TYPE_GRASS_6,
	TILE_TYPE_GRASS_7,
	TILE_TYPE_GRASS_8,

	// Row 2
	TILE_TYPE_GRASS_9,
	TILE_TYPE_GRASS_10,
	TILE_TYPE_GRASS_11,
	TILE_TYPE_DIRT_1,
	TILE_TYPE_DIRT_2,
	TILE_TYPE_DIRT_3,
	TILE_TYPE_DIRT_4,
	TILE_TYPE_SAND_1,

	// Row 3
	TILE_TYPE_STONE_1,
	TILE_TYPE_STONE_2,
	TILE_TYPE_SAND_2,
	TILE_TYPE_SAND_3,
	TILE_TYPE_MUD_1,
	TILE_TYPE_MUD_2,
	TILE_TYPE_DIRT_5,
	TILE_TYPE_SAND_4,

	// Row 4
	TILE_TYPE_STONE_3,
	TILE_TYPE_STONE_4,
	TILE_TYPE_STONE_5,
	TILE_TYPE_STONE_6,
	TILE_TYPE_STONE_7,
	TILE_TYPE_STONE_8,
	TILE_TYPE_STONE_9,
	TILE_TYPE_STONE_BRICK_1,

	// Row 5
	TILE_TYPE_STONE_BRICK_2,
	TILE_TYPE_STONE_BRICK_3,
	TILE_TYPE_STONE_BRICK_4,
	TILE_TYPE_STONE_BRICK_5,
	TILE_TYPE_STONE_BRICK_6,
	TILE_TYPE_STONE_BRICK_7,
	TILE_TYPE_STONE_BRICK_8,
	TILE_TYPE_STONE_BRICK_9,

	// Row 6
	TILE_TYPE_RED_BRICK_1,
	TILE_TYPE_RED_BRICK_2,
	TILE_TYPE_BLUE_BRICK,
	TILE_TYPE_RED_BRICK_3,
	TILE_TYPE_RED_BRICK_4,
	TILE_TYPE_STONE_BRICK_10,
	TILE_TYPE_STONE_10,
	TILE_TYPE_STONE_11,

	// Row 7
	TILE_TYPE_WOOD_FLOOR_VERTICAL,
	TILE_TYPE_WOOD_FLOOR_HORIZONTAL,
	TILE_TYPE_WOOD_TILE,
	TILE_TYPE_STONE_TILE,
	TILE_TYPE_WATER_1,
	TILE_TYPE_METAL_WALL,
	TILE_TYPE_WATER_2,
	TILE_TYPE_STONE_12,

	// Row 8
	TILE_TYPE_ENTRANCE,
	TILE_TYPE_EXIT,
	TILE_TYPE_GOLD_TILE,
	TILE_TYPE_VINE_1,
	TILE_TYPE_VINE_2,
	TILE_TYPE_SKY,
	TILE_TYPE_LAVA_1,
	TILE_TYPE_LAVA_2,

	NUM_TILE_TYPES
};


//-----------------------------------------------------------------------------------------------
struct TileDefinition;


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	IntVec2					m_tileCoords;
	TileType				m_type;

public:
	Tile( IntVec2 tileCoords, TileType type );
	~Tile();

	TileDefinition const&	GetDefinition() const;
	bool					IsSolid() const;
};