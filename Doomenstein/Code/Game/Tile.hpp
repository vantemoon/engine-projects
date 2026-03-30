#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class TileDefinition;


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	AABB3					m_bounds;
	TileDefinition const*	m_definition = nullptr;

public:
	Tile( AABB3 bounds, TileDefinition const* definition );
	~Tile();

	TileDefinition const&	GetDefinition() const;
	bool					IsSolid() const;
};