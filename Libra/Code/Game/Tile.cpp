#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<TileDefinition> TileDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeTileDefinitions()
{
	// Grass Tile
	s_definitions[TILE_TYPE_GRASS].m_isSolid = false;
	s_definitions[TILE_TYPE_GRASS].m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	s_definitions[TILE_TYPE_GRASS].m_tint = GRASS_COLOR;

	// Stone Tile
	s_definitions[TILE_TYPE_STONE].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE].m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	s_definitions[TILE_TYPE_STONE].m_tint = STONE_COLOR;
}


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