#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
struct MapDefinition
{
public:
	IntVec2	m_dimensions;
	float	m_aspectRatio = 1.f;
	Vec2	m_exitPosition = Vec2::ZERO;

	int m_numOfLeos = 0;
	int m_numOfScorpios = 0;
	int m_numOfAries = 0;

	TileType m_fillTileType = TILE_TYPE_GRASS_1;
	TileType m_borderTileType = TILE_TYPE_STONE_2;
	TileType m_bunkerFloorTileType = TILE_TYPE_STONE_2;
	TileType m_startbunkerWallTileType = TILE_TYPE_STONE_2;
	TileType m_exitbunkerWallTileType = TILE_TYPE_STONE_2;
	TileType m_sprinkle1TileType = TILE_TYPE_STONE_2;
	TileType m_sprinkle2TileType = TILE_TYPE_STONE_2;

	float m_sprinkle1Probability = 0.f;
	float m_sprinkle2Probability = 0.f;

	static std::vector<MapDefinition> s_definitions;

public:
	static void InitializeMapDefinitions();
};