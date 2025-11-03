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
	TestShapeTriangle*		m_triangle;
	TestShapeDisc*			m_disc;
	TestShapeAABB*			m_aabb;
	TestShapeOBB*			m_obb;
	TestShapeCapsule*		m_capsule;
	TestShapeLineSegment*	m_lineSegment;
	TestShapeLineSegment*	m_infiniteLine;

public:
	GameNearestPoint();
	~GameNearestPoint();

	void Update( float deltaSeconds ) override;
	void Render() const override;
};