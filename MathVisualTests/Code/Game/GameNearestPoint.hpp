#pragma once
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class TestShapeTriangle;
class TestShapeDisc;
class TestShapeAABB;
class TestShapeOBB;
class TestShapeCapsule;
class TestShapeLineSegment;


//-----------------------------------------------------------------------------------------------
class GameNearestPoint : public Game
{
public:
	TestShapeDisc*			m_referencePoint;

	TestShapeTriangle*		m_triangle;
	TestShapeDisc*			m_disc;
	TestShapeAABB*			m_aabb;
	TestShapeOBB*			m_obb;
	TestShapeCapsule*		m_capsule;
	TestShapeLineSegment*	m_lineSegment;
	TestShapeLineSegment*	m_infiniteLine;

	TestShapeDisc*			m_nearestPointOnTriangle;
	TestShapeDisc*			m_nearestPointOnDisc;
	TestShapeDisc*			m_nearestPointOnAABB;
	TestShapeDisc*			m_nearestPointOnOBB;
	TestShapeDisc*			m_nearestPointOnCapsule;
	TestShapeDisc*			m_nearestPointOnLineSegment;
	TestShapeDisc*			m_nearestPointOnInfiniteLine;

	bool                    m_isSlowMo = false;

public:
	GameNearestPoint();
	~GameNearestPoint();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard();
	void Render() const override;

private:
	void GetNearestPoints();
};