#include "Game/MapDefinition.hpp"
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<MapDefinition> MapDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void MapDefinition::InitializeMapDefinitions()
{
	// Map 1
	IntVec2 exitTileCoords1 = IntVec2( 31, 63 );
	Vec2 exitPos1 = Vec2( ( exitTileCoords1.x + 0.5f ) * TILE_SIZE, ( exitTileCoords1.y + 0.5f ) * TILE_SIZE );

	s_definitions[0].m_dimensions = IntVec2( 32, 64 );
	s_definitions[0].m_aspectRatio = 0.5f;
	s_definitions[0].m_exitPosition = exitPos1;

	s_definitions[0].m_numOfLeos = 5;
	s_definitions[0].m_numOfScorpios = 5;
	s_definitions[0].m_numOfAries = 5;

	s_definitions[0].m_fillTileType = TILE_TYPE_GRASS_1;
	s_definitions[0].m_borderTileType = TILE_TYPE_STONE_2;
	s_definitions[0].m_bunkerFloorTileType = TILE_TYPE_STONE_TILE;
	s_definitions[0].m_startbunkerWallTileType = TILE_TYPE_STONE_BRICK_11;
	s_definitions[0].m_exitbunkerWallTileType = TILE_TYPE_STONE_BRICK_4;
	s_definitions[0].m_sprinkle1TileType = TILE_TYPE_STONE_2;
	s_definitions[0].m_sprinkle2TileType = TILE_TYPE_STONE_2; // Not used

	s_definitions[0].m_sprinkle1Probability = 0.1f;
	s_definitions[0].m_sprinkle2Probability = 0.f;

	// Map 2
	IntVec2 exitTileCoords2 = IntVec2( 49, 19 );
	Vec2 exitPos2 = Vec2( ( exitTileCoords2.x + 0.5f ) * TILE_SIZE, ( exitTileCoords2.y + 0.5f ) * TILE_SIZE );

	s_definitions[1].m_dimensions = IntVec2( 50, 20 );
	s_definitions[1].m_aspectRatio = 2.5f;
	s_definitions[1].m_exitPosition = exitPos2;

	s_definitions[1].m_numOfLeos = 5;
	s_definitions[1].m_numOfScorpios = 5;
	s_definitions[1].m_numOfAries = 3;

	s_definitions[1].m_fillTileType = TILE_TYPE_GRASS_4;
	s_definitions[1].m_borderTileType = TILE_TYPE_STONE_BRICK_4;
	s_definitions[1].m_bunkerFloorTileType = TILE_TYPE_STONE_TILE;
	s_definitions[1].m_startbunkerWallTileType = TILE_TYPE_STONE_BRICK_4;
	s_definitions[1].m_exitbunkerWallTileType = TILE_TYPE_STONE_BRICK_11;
	s_definitions[1].m_sprinkle1TileType = TILE_TYPE_STONE_2;
	s_definitions[1].m_sprinkle2TileType = TILE_TYPE_MUD_2;

	s_definitions[1].m_sprinkle1Probability = 0.1f;
	s_definitions[1].m_sprinkle2Probability = 0.1f;

	// Map 3
	IntVec2 exitTileCoords3 = IntVec2( 31, 31 );
	Vec2 exitPos3 = Vec2( ( exitTileCoords3.x + 0.5f ) * TILE_SIZE, ( exitTileCoords3.y + 0.5f ) * TILE_SIZE );

	s_definitions[2].m_dimensions = IntVec2( 32, 32 );
	s_definitions[2].m_aspectRatio = 1.f;
	s_definitions[2].m_exitPosition = exitPos3;

	s_definitions[2].m_numOfLeos = 5;
	s_definitions[2].m_numOfScorpios = 5;
	s_definitions[2].m_numOfAries = 10;

	s_definitions[2].m_fillTileType = TILE_TYPE_MUD_2;
	s_definitions[2].m_borderTileType = TILE_TYPE_STONE_BRICK_11;
	s_definitions[2].m_bunkerFloorTileType = TILE_TYPE_STONE_TILE;
	s_definitions[2].m_startbunkerWallTileType = TILE_TYPE_STONE_BRICK_11;
	s_definitions[2].m_exitbunkerWallTileType = TILE_TYPE_STONE_BRICK_11;
	s_definitions[2].m_sprinkle1TileType = TILE_TYPE_STONE_BRICK_11;
	s_definitions[2].m_sprinkle2TileType = TILE_TYPE_GRASS_4;

	s_definitions[2].m_sprinkle1Probability = 0.1f;
	s_definitions[2].m_sprinkle2Probability = 0.2f;
}