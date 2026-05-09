#pragma once
#include "Game/Game.hpp"
#include "Game/TestShape.hpp"
#include "Game/TestShapeAABB3D.hpp"
#include "Game/TestShapeOBB3D.hpp"
#include "Game/TestShapeSphere.hpp"
#include "Game/TestShapePlane3D.hpp"
#include "Game/TestShapeZCylinder.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Game3DShapes : public Game
{
public:
	const static int	MAX_SHAPES = 12;
	const static int	NUM_SHAPE_PER_TYPE = 4;
	const static int    NUM_PLANE = 1;

	TestShapeAABB3D*	m_testAABBs[NUM_SHAPE_PER_TYPE] = {};
	TestShapeSphere*	m_testSpheres[NUM_SHAPE_PER_TYPE] = {};
	TestShapeZCylinder* m_testCylinders[NUM_SHAPE_PER_TYPE] = {};
	TestShapeOBB3D*		m_testOBBs[NUM_SHAPE_PER_TYPE] = {};
	TestShapePlane3D*	m_testPlanes[NUM_PLANE] = {};

	Vec3				m_refPoint = Vec3::ZERO;
	Vec3                m_refDirection = Vec3::ZERO;
	bool                m_isUsingCameraAsRefPoint = true;

	std::vector<Vec3>	m_nearestPointsToCamera;
	std::vector<Vec3>	m_nearestPointsToRefPoint;
	Vec3				m_nearestPoint = Vec3::ZERO;

	float				m_raycastMaxLength = 10.f;
	RaycastResult3D		m_nearestRaycastResult;
	int					m_impactedShapeType = -1;
	int					m_impactedShapeIndex = -1;

	bool				m_isShapeGrabbed = false;
	int					m_grabbedShapeType = -1;
	int					m_grabbedShapeIndex = -1;
	Vec3				m_grabbedShapeLocalPosition = Vec3::ZERO;

	Timer*				m_overlapPulseTimer = nullptr;

	Rgba8				m_darkBlue = Rgba8( 50, 80, 150, 255 );
	Rgba8				m_lightBlue = Rgba8( 100, 150, 255, 255 );

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
	void GenerateRandomOBBs();
	void GenerateRandomPlanes();

	void GetNearestPoints();
	void GetNearestPointsToCamera();
	void GetNearestPointsToRefPoint();

	void CheckIfShapesOverlap();

	void RaycastAgainstShapes();

	void HandleShapeGrabbing();
	void UpdateGrabbedShapeFromCamera();
	void ClearGrabbedShape();

	bool IsShapeGrabbed( int shapeType, int shapeIndex ) const;
	Vec3 GetShapeCenterWorldPosition( int shapeType, int shapeIndex ) const;
	void SetShapeCenterWorldPosition( int shapeType, int shapeIndex, Vec3 const& worldCenter );
};