#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"


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
TileDefinition const& Tile::GetDefinition() const
{
	int typeIndex = static_cast<int>( m_type );
	return TileDefinition::s_definitions[typeIndex];
}


//-----------------------------------------------------------------------------------------------
bool Tile::IsSolid() const
{
	const TileDefinition& tileDef = GetDefinition();
	return tileDef.m_isSolid;
}