#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeAABB3D.hpp"
#include "Game/TestShapeSphere.hpp"
#include "Game/TestShapeZCylinder.hpp"
#include "Engine/Core/Timer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Game3DShapes : public Game
{
public:
	const static int	MAX_SHAPES = 12;
	const static int	NUM_SHAPE_PER_TYPE = 4;

	TestShapeAABB3D*	m_testAABBs[NUM_SHAPE_PER_TYPE] = {};
	TestShapeSphere*	m_testSpheres[NUM_SHAPE_PER_TYPE] = {};
	TestShapeZCylinder* m_testCylinders[NUM_SHAPE_PER_TYPE] = {};

	bool                m_isUsingCameraAsRefPoint = true;
	std::vector<Vec3>	m_nearestPointsToCamera;
	std::vector<Vec3>	m_nearestPointsToRefPoint;
	Vec3				m_nearestPoint = Vec3::ZERO;

	Timer*				m_overlapPulseTimer = nullptr;

	Game3DShapes();
	~Game3DShapes();

	void Update( float deltaSeconds ) override;
	void UpdateFromKeyboard( float deltaSeconds );
	void Render() const override;

private:
	void GenerateRandomShapes();
	void GenerateRandomAABBs();
	void GenerateRandomSpheres();
	void GenerateRandomCylinders();

	void GetNearestPoints();
	void GetNearestPointsToCamera();
	void GetNearestPointsToRefPoint();

	void CheckIfShapesOverlap();
};