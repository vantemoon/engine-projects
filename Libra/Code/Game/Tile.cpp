#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( IntVec2 tileCoords, TileDefinition const* definition )
	: m_tileCoords( tileCoords )
	, m_definition( definition )
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
	return *m_definition;
}


//-----------------------------------------------------------------------------------------------
bool Tile::IsSolid() const
{
	const TileDefinition& tileDef = GetDefinition();
	return tileDef.m_isSolid;
}