#pragma once
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
typedef std::vector<Entity*> EntityList;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	std::vector<Tile>		m_tiles;
	EntityList				m_allEntities;
	IntVec2					m_dimensions;

public:
	Map( IntVec2 dimensions );
	~Map();
};