#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( IntVec2 tileCoords, TileDefinition const* definition )
	: m_tileCoords( tileCoords )
	, m_definition( definition )
{
}


//-----------------------------------------------------------------------------------------------
Tile::~Tile()
{
}


//-----------------------------------------------------------------------------------------------
TileDefinition const& Tile::GetDefinition() const
{
	return *m_definition;
}


//-----------------------------------------------------------------------------------------------
bool Tile::IsSolid() const
{
	return m_definition->m_isSolid;
}