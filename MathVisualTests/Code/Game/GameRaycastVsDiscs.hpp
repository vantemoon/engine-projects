#pragma once
#include "Game/Game.hpp"
#include "Game/TestShapeDisc.hpp"
#include "Engine/Math/RaycastUtils.hpp"


//-----------------------------------------------------------------------------------------------
class GameRaycastVsDiscs : public Game
{
public:
	const static int MAX_DISCS = 10;
	TestShapeDisc* m_testDiscs[MAX_DISCS] = {};

	Vec2    m_rayStartPos = Vec2( 50, 25 );
	Vec2    m_rayEndPos = Vec2( 150, 75 );

	int     m_closestImpactDiscIndex = -1;
	RaycastResult2D m_closestImpactResult;

	bool                    m_isSlowMo = false;

	GameRaycastVsDiscs();
	~GameRaycastVsDiscs();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void Render() const override;

private:
	void GenerateRandomDiscs();
	void RaycastVsDiscs();
	Vec2 GetMouseWorldPos() const;
};