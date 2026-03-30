#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( AABB3 bounds, TileDefinition const* definition )
	: m_bounds( bounds )
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