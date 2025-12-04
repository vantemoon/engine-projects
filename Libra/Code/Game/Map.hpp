#pragma once
#include "Game/Entity.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/TileHeatMap.hpp"
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
struct MapDefinition;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	std::vector<Tile>		m_tiles;
	EntityList				m_allEntities;
	EntityList				m_entityListsByType[NUM_ENTITY_TYPES];
	EntityList              m_entityListsByFaction[NUM_ENTITY_FACTIONS];
	IntVec2					m_dimensions = IntVec2::ZERO;
	MapDefinition const*	m_definition = nullptr;
	int                     m_index = -1;

public:
	Map( MapDefinition const* definition, int index );
	~Map();

	void				 Update( float deltaSeconds );
	void				 UpdateEntities( float deltaSeconds );
	void				 Render()												const;
	void				 RenderTiles()											const;
	void				 RenderEntities()										const;
	void				 DebugRender() 											const;
	void				 UpdateWorldCameraView()								const;

	Tile*				 GetTile( IntVec2 tileCoords )							const;
	AABB2				 GetTileBounds( IntVec2 tileCoords )					const;
	bool				 IsTileCoordsInBounds( IntVec2 tileCoords )				const;
	IntVec2				 GetTileCoordsForWorldPosition( Vec2 worldPos )			const;
	Vec2				 GetWorldPositionForTileCoords( IntVec2 tileCoords )	const;

	bool				 IsPointInSolidTile( Vec2 const& point )				const;
	bool				 IsTileSolid( Tile tile, bool treatWaterAsSolid ) const;
	bool                 IsTileOpaque( Tile tile )								const;
	bool                 IsTileBlockedForDijkstra( IntVec2 const& coords, bool treatWaterAsSolid, 
												   std::vector<bool> const& scorpioBlocked ) const;

	RaycastResult2D		 RaycastVsTiles( Vec2 const& startPos, Vec2 const& fwdNormal, float maxDist, float stepSize ) const;
	bool				 HasLineOfSight( Vec2 const& startPos, Vec2 const& endPos, float stepSize ) const;

	void                 SpawnEntitiesForMapDefinition();
	void                 SpawnExplosionAtPosition( Vec2 const& position, float duration = 0.5f, float scale = 10.f, bool orientRandomly = true, float orientationDegrees = 0.f );
	Entity*				 SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees, float duration = 0.5f, float scale = 10.f );
	void				 AddEntityToMap( Entity& entity, EntityType type, EntityFaction faction );
	void				 RemoveEntityFromMap( Entity& entity, EntityType type, EntityFaction faction );

	MapDefinition const& GetDefinition()										const;

	void                 BuildScorpioBlockedMask( std::vector<bool>& out_mask ) const;
	void				 PopulateDijkstraMap( TileHeatMap& out_dijkstraMap, IntVec2 startCoords, float maxCost, bool treatWaterAsSolid = true ) const;
	void                 PopulateEnemyReachabilityMap( TileHeatMap& out_reachabilityMap, IntVec2 startCoords, float maxCost, bool treatWaterAsSolid = true ) const;

private:
	void				 PopulateMap();
	void                 GenerateTiles();
	bool                 IsMapValid() const;
	void                 FillUnreachableTiles();
	void				 DeleteGarbageEntities();
	void				 ResolveEntityVsEntityCollision();
	void				 ResolveEntityVsTileCollision();
	bool                 IsAnyEntityAtTile( IntVec2 tileCoords ) const;
};