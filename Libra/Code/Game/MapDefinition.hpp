#pragma once
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
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

	TileDefinition const* m_fillTileType = nullptr;
	TileDefinition const* m_borderTileType = nullptr;
	TileDefinition const* m_bunkerFloorTileType = nullptr;
	TileDefinition const* m_startbunkerWallTileType = nullptr;
	TileDefinition const* m_exitbunkerWallTileType = nullptr;

	TileDefinition const* m_worm1TileType = nullptr;
	TileDefinition const* m_worm2TileType = nullptr;

	int m_numOfWorms1 = 0;
	int m_numOfWorms2 = 0;
	int m_wormLength1 = 0;
	int m_wormLength2 = 0;

	static std::vector<MapDefinition> s_definitions;

public:
	static void InitializeMapDefinitions();
};