#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct TileDefinition;


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	IntVec2					m_tileCoords;
	TileDefinition const*	m_definition = nullptr;

public:
	Tile( IntVec2 tileCoords, TileDefinition const* definition );
	~Tile();

	TileDefinition const&	GetDefinition() const;
	bool					IsSolid() const;
};