#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<TileDefinition> TileDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeTileDefinitions()
{
	// Grass Tile
	s_definitions[TILE_TYPE_GRASS_1].m_isSolid = false;
	s_definitions[TILE_TYPE_GRASS_1].m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	s_definitions[TILE_TYPE_GRASS_1].m_tint = GRASS_COLOR;

	// Stone Tile
	s_definitions[TILE_TYPE_STONE_2].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE_2].m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	s_definitions[TILE_TYPE_STONE_2].m_tint = STONE_COLOR;
}