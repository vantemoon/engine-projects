#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/TileHeatMap.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//-----------------------------------------------------------------------------------------------
class Leo : public Entity
{
public:
	Vec2    m_goalPosition = Vec2::ZERO;
	Vec2	m_movementTargetPosition = Vec2::ZERO;
	float	m_timeSinceLastFire = 9999.f;
	float   m_timeSinceLastTurn = 0.f;

	TileHeatMap* m_dijkstraMap = nullptr;
	IntVec2      m_dijkstraOriginTile = IntVec2( -1, -1 );
	Vec2         m_lastSeenPlayerPosition = Vec2::ZERO;
	Vec2         m_waypointPosition = Vec2::ZERO;
	Vec2		 m_wanderGoalPosition = Vec2::ZERO;
	IntVec2		 m_wanderGoalTile = IntVec2( -1, -1 );

	RandomNumberGenerator m_rng;

public:
	Leo( Vec2 startingPosition, float orientationDegrees );
	~Leo();

	void Update( float deltaSeconds )                          override;
	void Render()                                        const override;
	void TakeDamage( int damage )                              override;
	void Die()                                                 override;

protected:
	void InitializeVertexArray()                               override;
	void PickNewWanderGoal();
	void UpdateDijkstraMap();
	void UpdateWaypointChase();
	void UpdateWaypointWander();
	void MoveTowardTargetPosition( float deltaSeconds );
	void FireProjectile();

	bool IsPlayerInAdjacentTile() const;
	bool IsTileOccupiedByScorpio( const IntVec2& tileCoords ) const;
	bool IsTileTraversableForWander( const IntVec2& tileCoords ) const;
};