#pragma once
#include "Game/Game.hpp"
#include "Game/TestShapeAABB.hpp"
#include "Engine/Math/RaycastUtils.hpp"


//-----------------------------------------------------------------------------------------------
class GameRaycastVsAABB : public Game
{
	public:
	const static int			 MAX_AABBS = 10;
	const static int			 MAX_BOUNCES = 100;
	inline const static float    RAY_BOUNCE_RESTART_OFFSET = 0.01f;

	TestShapeAABB*			m_testAABBs[MAX_AABBS] = {};

	Vec2					m_rayStartPos = Vec2( 50, 25 );
	Vec2					m_rayEndPos = Vec2( 150, 75 );
	int						m_numRaySegments = 0;
	RaycastResult2D			m_raySegmentResults[MAX_BOUNCES + 1] = {};
	int						m_raySegmentDidHit[MAX_BOUNCES + 1] = {};
	bool					m_aabbWasImpacted[MAX_AABBS] = {};

	bool					m_isSlowMo = false;

	GameRaycastVsAABB();
	~GameRaycastVsAABB();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void Render() const override;

private:
	void GenerateRandomAABBs();
	void RaycastVsAABBs();
	Vec2 GetMouseWorldPos() const;
};