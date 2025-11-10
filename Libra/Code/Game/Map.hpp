#pragma once
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct AABB2;


//-----------------------------------------------------------------------------------------------
struct RaycastResult2D
{
	// Basic raycast result information (required)
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;

	// Original raycast information (optional)
	Vec2	m_rayStartPos;
	Vec2	m_rayFwdNormal;
	float	m_rayMaxLength = 1.f;
};


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	std::vector<Tile>		m_tiles;
	EntityList				m_allEntities;
	EntityList				m_entityListsByType[NUM_ENTITY_TYPES];
	EntityList              m_entityListsByFaction[NUM_ENTITY_FACTIONS];
	IntVec2					m_dimensions;

public:
	Map( IntVec2 dimensions );
	~Map();

	void			Update( float deltaSeconds );
	void			UpdateEntities( float deltaSeconds );
	void			Render()										const;
	void			RenderTiles()									const;
	void			RenderEntities()								const;
	void			DebugRender() 									const;
	void			UpdateWorldCameraView()							const;

	Tile*			GetTile( IntVec2 tileCoords )					const;
	AABB2			GetTileBounds( IntVec2 tileCoords )				const;
	bool			IsTileCoordsInBounds( IntVec2 tileCoords )		const;
	IntVec2			GetTileCoordsForWorldPosition( Vec2 worldPos )	const;
	Vec2			GetWorldPositionForTileCoords( IntVec2 tileCoords )	const;

	bool			IsPointInSolidTile( Vec2 const& point )				const;
	bool			IsTileSolid( Tile tile )						const;

	RaycastResult2D RaycastVsTiles( Vec2 const& startPos, Vec2 const& fwdNormal, float maxDist, float stepSize ) const;
	bool            HasLineOfSight( Vec2 const& startPos, Vec2 const& endPos, float stepSize ) const;

	Entity*			SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees );
	void			AddEntityToMap( Entity& entity, EntityType type, EntityFaction faction );
	void			RemoveEntityFromMap( Entity& entity, EntityType type, EntityFaction faction );

private:
	void			PopulateTiles();
	void			DeleteGarbageEntities();
	void			ResolveEntityVsEntityCollision();
	void			ResolveEntityVsTileCollision();
};