#pragma once
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct AABB2;


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

	void Update( float deltaSeconds );
	void UpdateEntities( float deltaSeconds );
	void Render()											const;
	void RenderTiles()										const;
	void RenderEntities()									const;
	void DebugRender() 										const;

	Tile*   GetTile( IntVec2 tileCoords )					const;
	AABB2   GetTileBounds( IntVec2 tileCoords )				const;
	bool    IsTileCoordsInBounds( IntVec2 tileCoords )		const;
	IntVec2 GetTileCoordsForWorldPosition( Vec2 worldPos )	const;

private:
	void PopulateTiles();
	void DeleteGarbageEntities();
};