#include "Game//Tile.hpp"


//-----------------------------------------------------------------------------------------------
TileDef TileDef::s_tileDefs[ NUM_TILE_TYPES ];


//-----------------------------------------------------------------------------------------------
void TileDef::InitializeTileDefs()
{
	// Grass Tile
	s_tileDefs[TILE_TYPE_GRASS].m_isSolid = false;
	s_tileDefs[TILE_TYPE_GRASS].m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	s_tileDefs[TILE_TYPE_GRASS].m_tint = Rgba8( 255, 255, 255 );

	// Stone Tile
	s_tileDefs[TILE_TYPE_STONE].m_isSolid = true;
	s_tileDefs[TILE_TYPE_STONE].m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	s_tileDefs[TILE_TYPE_STONE].m_tint = Rgba8( 200, 200, 200 );
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


//-----------------------------------------------------------------------------------------------
bool Tile::IsSolid() const
{
	if ( m_type == TileType::TILE_TYPE_STONE )
	{
		return true;
	}
	return false;
}