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
	int						m_health = 0; // Per-tile health

public:
	Tile( IntVec2 tileCoords, TileDefinition const* definition );
	~Tile();

	TileDefinition const&	GetDefinition() const;
	bool					IsSolid() const;
	bool					IsDestructible() const;
	void					TakeDamage( int damage );
	void					SetType( TileDefinition const* newDefinition );
};