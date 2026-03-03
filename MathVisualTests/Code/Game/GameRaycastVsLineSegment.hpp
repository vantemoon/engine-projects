#pragma once
#include "Game/Game.hpp"
#include "Game/TestShapeLineSegment.hpp"
#include "Engine/Math/RaycastUtils.hpp"


//-----------------------------------------------------------------------------------------------
class GameRaycastVsLineSegment : public Game
{
public:
	const static int	MAX_LINE_SEGMENTS = 10;
	TestShapeLineSegment* m_testLineSegments[MAX_LINE_SEGMENTS] = {};

	Vec2					m_rayStartPos = Vec2( 50, 25 );
	Vec2					m_rayEndPos = Vec2( 150, 75 );

	int						m_closestImpactLineSegmentIndex = -1;
	RaycastResult2D			m_closestImpactResult;

	bool					m_isSlowMo = false;

	GameRaycastVsLineSegment();
	~GameRaycastVsLineSegment();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void Render() const override;

private:
	void GenerateRandomLineSegments();
	void RaycastVsLineSegments();
	Vec2 GetMouseWorldPos() const;
};