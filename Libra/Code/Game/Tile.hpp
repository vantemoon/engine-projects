#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
enum TileType
{
	TILE_TYPE_INVALID = -1,
	TILE_TYPE_GRASS,
	TILE_TYPE_STONE,
	NUM_TILE_TYPES
};


//-----------------------------------------------------------------------------------------------
class TileDef
{
public:
	// std::string	m_name;
	bool		m_isSolid;
	AABB2		m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	Rgba8		m_tint = Rgba8( 255, 255, 255 );

	static TileDef s_tileDefs[NUM_TILE_TYPES];

public:
	static void InitializeTileDefs();
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