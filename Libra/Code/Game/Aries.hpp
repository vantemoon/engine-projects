#pragma once
#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Core/TileHeatMap.hpp"


//-----------------------------------------------------------------------------------------------
class Aries : public Entity
{
public:
	Vec2    m_goalPosition = Vec2::ZERO;
	Vec2    m_movementTargetPosition = Vec2::ZERO;

	TileHeatMap* m_dijkstraMap = nullptr;
	IntVec2      m_dijkstraOriginTile = IntVec2( -1, -1 );
	Vec2         m_lastSeenPlayerPosition = Vec2::ZERO;
	Vec2         m_waypointPosition = Vec2::ZERO;
	Vec2         m_wanderGoalPosition = Vec2::ZERO;
	IntVec2      m_wanderGoalTile = IntVec2( -1, -1 );

	float   m_timeSinceLastTurn = 0.f;

public:
	Aries( Vec2 startingPosition, float orientationDegrees );
	~Aries();

	void Update( float deltaSeconds )                          override;
	void Render()                                        const override;
	void TakeDamage( int damage )                              override;
	void Die()                                                 override;
	void ReflectBullet( Bullet& bullet );

protected:
	void InitializeVertexArray()                               override;
	void MoveTowardTargetPosition( float deltaSeconds );

	void PickNewWanderGoal();
	void UpdateDijkstraMap();
	void UpdateWaypointChase();
	void UpdateWaypointWander();

	bool IsPlayerInAdjacentTile() const;
	bool IsTileOccupiedByScorpio( const IntVec2& tileCoords ) const;
	bool IsTileTraversableForWander( const IntVec2& tileCoords ) const;
};
